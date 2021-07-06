/*
 *  ____           _
 * |  _ \ ___   __| |_ __ _   _ _ __ ___
 * | |_) / _ \ / _` | '__| | | | '_ ` _ \
 * |  __/ (_) | (_| | |  | |_| | | | | | |
 * |_|   \___/ \__,_|_|   \__,_|_| |_| |_|
 *
 * Copyright 2021 Podrum Team.
 *
 * This file is licensed under the GPL v2.0 license.
 * The license file is located in the root directory
 * of the source code. If not you may not use this file.
 *
 */

#include <Python.h>
#include <math.h>

typedef struct {
	char *buffer;
	int size;
} pack_t;


int sign_var_int(unsigned int value) {
	return value >= 0 ? (value << 1) : ((((-1 * value) - 1) << 1) | 1);
}

pack_t c_block_storage_network_serialize(int blocks[], int palette[], int palette_length) {
	char *result = malloc(1);
	int size = 1;
	int bits_per_block = ceil(log2(palette_length));
	if (bits_per_block <= 0) {
		bits_per_block = 1;
	}
	char bits[8] = {1, 2, 3, 4, 5, 6, 8, 16};
        int i, ii, word, state;
        for (i = 0; i < 8; ++i) {
		if (bits[i] >= bits_per_block) {
			bits_per_block = bits[i];
			break;
		}
	}
	result[0] = (bits_per_block << 1) | 1;
        int blocks_per_word = floor(32 / bits_per_block);
	int words_per_chunk = ceil(4096 / blocks_per_word);
	int offset = 1;
	int pos = 0;
	for (i = 0; i < words_per_chunk; ++i) {
		word = 0;
		for (ii = 0; ii < blocks_per_word; ++ii) {
			if (pos >= 4096) {
				break;
			}
			state = blocks[pos];
			word |= state << (bits_per_block * ii);
			++pos;
		}
		size += 4;
		result = realloc(result, size * sizeof(char));
		result[offset] = word & 0xff;
                ++offset;
		result[offset] = (word >> 8) & 0xff;
		++offset;
		result[offset] = (word >> 16) & 0xff;
		++offset;
		result[offset] = (word >> 24) & 0xff;
		++offset;
	}
        int value;
	unsigned char to_write;
	value = sign_var_int(palette_length) & 0xffffffff;
	for (ii = 0; ii < 5; ++ii) {
		to_write = value & 0x7f;
		value >>= 7;
		size += 1;
		result = realloc(result, size * sizeof(char));
		if (value != 0) {
			result[offset] = to_write | 0x80;
			++offset;
		} else {
			result[offset] = to_write;
			++offset;
			break;
		}
	}
	for (i = 0; i < palette_length; ++i) {
		value = sign_var_int(palette[i]) & 0xffffffff;
		for (ii = 0; ii < 5; ++ii) {
			to_write = value & 0x7f;
			value >>= 7;
			size += 1;
			result = realloc(result, size * sizeof(char));
			if (value != 0) {
				result[offset] = to_write | 0x80;
				++offset;
			} else {
				result[offset] = to_write;
				++offset;
				break;
			}
		}
	}
	pack_t out;
	out.buffer = result;
	out.size = size;
	return out;
}

static PyObject *block_storage_network_serialize(PyObject *self, PyObject *args)
{
	PyObject *blocks_obj;
	PyObject *palette_obj;
	PyObject *long_obj;
	int palette_length;
	if (!PyArg_ParseTuple(args, "OOi", &blocks_obj, &palette_obj, &palette_length)) {
		return NULL;
	}
        int blocks[4096];
        int i;
        for (i = 0; i < 4096; ++i) {
		long_obj = PyList_GetItem(blocks_obj, i);
		blocks[i] = PyLong_AsLong(long_obj);
	}
	int *palette = malloc(palette_length * sizeof(int));
	for (i = 0; i < palette_length; ++i) {
		long_obj = PyList_GetItem(palette_obj, i);
		palette[i] = PyLong_AsLong(long_obj);
	}
	pack_t result = c_block_storage_network_serialize(blocks, palette, palette_length);
	return PyBytes_FromStringAndSize(result.buffer, result.size);
}

static PyMethodDef myMethods[] = {
	{
		"block_storage_network_serialize",
		block_storage_network_serialize,
		METH_VARARGS,
		"Serialises network block storages."
	}
};

static struct PyModuleDef myModule = {
	PyModuleDef_HEAD_INIT,
	"chunk_utils",
	"Podrum's chunk utilities.",
	-1,
	myMethods
};

PyMODINIT_FUNC PyInit_chunk_utils(void)
{
	return PyModule_Create(&myModule);
}
