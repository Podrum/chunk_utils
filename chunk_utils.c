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

int perlin_grad(int hash, int x, int y, int z) {
	int u, v;
	hash &= 15;
	if (hash & 8) {
		u = y;
	} else {
		u = x;
	}
	if (hash & 12) {
		if (hash == 12 || hash == 14) {
			v = x;
		} else {
			v = z;
		}
	} else {
		v = y;
	}
	return ((hash & 1) ? u : (-1 * u)) + ((hash & 2) ? v : (-1 * v));
}

int perlin_fade(int t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

int perlin_lerp(int t, int a, int b) {
	return a + t * (b - a);
}

int perlin_noise(int x, int y, int z, int grad, int fade, int lerp, int m, int *p) {
	int xf = (int) floor(x);
	int yf = (int) floor(y);
	int zf = (int) floor(z);
	int xm = xf % m;
	int ym = yf % m;
	int zm = zf % m;
	x -= xf;
	y -= yf;
	z -= zf;
	int u = perlin_fade(x);
	int v = perlin_fade(y);
	int w = perlin_fade(z);
	int a = p[xm] + ym;
	int aa = p[a] + zm;
	int ab = p[a + 1] + zm;
	int b = p[xm + 1] + ym;
	int ba = p[b] + zm;
	int bb = p[b + 1] + zm;
	return perlin_lerp(w, perlin_lerp(v, perlin_lerp(u, perlin_grad(p[aa], x, y, z), perlin_grad(p[ba], x - 1, y, z)),
	       perlin_lerp(u, perlin_grad(p[ab], x, y - 1, z), perlin_grad(p[bb], x - 1, y - 1, z))),
	       perlin_lerp(v, perlin_lerp(u, perlin_grad(p[aa + 1], x, y, z - 1), perlin_grad(p[ba + 1], x - 1, y, z - 1)),
	       perlin_lerp(u, perlin_grad(p[ab + 1], x, y - 1, z - 1), perlin_grad(p[bb + 1], x - 1, y - 1, z - 1))));
}

typedef struct {
	unsigned char *buffer;
	int size;
} pack_t;

void put_var_int(unsigned int value, pack_t *pack) {
	value &= 0xffffffff;
	for (int i = 0; i < 5; ++i) {
		unsigned char to_write = value & 0x7f;
		value >>= 7;
		pack->buffer = realloc(pack->buffer, (pack->size + 1) * sizeof(char));
		if (value != 0) {
			pack->buffer[pack->size] = (to_write | 0x80);
			++pack->size;
		} else {
			pack->buffer[pack->size] = to_write;
			++pack->size;
			break;
		}
	}
}

void put_signed_var_int(int value, pack_t *pack) {
	put_var_int((value << 1) ^ (value >> 31), pack);
}

void put_unsigned_int_le(unsigned int value, pack_t *pack) {
	pack->buffer = realloc(pack->buffer, (pack->size + 4) * sizeof(char));
	pack->buffer[pack->size] = value & 0xff;
	++pack->size;
	pack->buffer[pack->size] = (value >> 8) & 0xff;
	++pack->size;
	pack->buffer[pack->size] = (value >> 16) & 0xff;
	++pack->size;
	pack->buffer[pack->size] = (value >> 24) & 0xff;
	++pack->size;
}

pack_t c_block_storage_network_serialize(unsigned int blocks[], int *palette, int palette_length) {
	pack_t out;
	out.buffer = malloc(1);
	out.size = 1;
	int bits_per_block = (int) ceil(log2(palette_length));
	int bits[8] = {1, 2, 3, 4, 5, 6, 8, 16};
        for (int i = 0; i < 8; ++i) {
		if (bits[i] >= bits_per_block) {
			bits_per_block = bits[i];
			break;
		}
	}
	out.buffer[0] = (bits_per_block << 1) | 1;
	int blocks_per_word = (int) floor(32 / bits_per_block);
	int words_per_chunk = (int) ceil(4096 / blocks_per_word);
	int pos = 0;
	for (int chunk_index = 0; chunk_index < words_per_chunk; ++chunk_index) {
		unsigned int word = 0;
		for (int block_index = 0; block_index < blocks_per_word; ++block_index) {
			if (pos >= 4096) {
				break;
			}
			unsigned int state = blocks[pos];
			word |= state << (bits_per_block * block_index);
			++pos;
		}
		put_unsigned_int_le(word, &out);
	}
	put_signed_var_int(palette_length, &out);
	for (int i = 0; i < palette_length; ++i) {
		put_signed_var_int(palette[i], &out);
	}
	return out;
}

static PyObject *block_storage_network_serialize(PyObject *self, PyObject *args)
{
	PyObject *blocks_obj;
	PyObject *palette_obj;
	PyObject *long_obj;
	if (!PyArg_ParseTuple(args, "OO", &blocks_obj, &palette_obj)) {
		return NULL;
	}
	unsigned int blocks[4096];
	for (int i = 0; i < 4096; ++i) {
		long_obj = PyList_GetItem(blocks_obj, i);
		blocks[i] = PyLong_AsLong(long_obj);
	}
	int palette_length = (int) PyList_Size(palette_obj);
	int *palette = malloc(palette_length * sizeof(int));
	for (int i = 0; i < palette_length; ++i) {
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
	},
	{
		NULL,
		NULL,
		0,
		NULL
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
