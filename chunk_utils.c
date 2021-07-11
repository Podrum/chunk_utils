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

#include "binary_stream.h"
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

binary_stream_t c_block_storage_network_serialize(unsigned int blocks[], int *palette, int palette_length) {
	binary_stream_t stream;
	stream.buffer = malloc(0);
	stream.offset = 0;
	stream.size = 0;
	int bits_per_block = (int) ceil(log2(palette_length));
	switch (bits_per_block)
	{
		case 0:
			bits_per_block = 1;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			break;
		case 7:
		case 8:
			bits_per_block = 8;
		default:
			if (bits_per_block > 8) {
				bits_per_block = 16;
			}
			break;
	}
	put_unsigned_byte((char) ((bits_per_block << 1) | 1), &stream);
	int blocks_per_word = (int) floor(32.0 / bits_per_block);
	int words_per_chunk = (int) ceil(4096.0 / blocks_per_word);
	int pos = 0;
	for (int chunk_index = 0; chunk_index < words_per_chunk; ++chunk_index) {
		unsigned int word = 0;
		for (int block_index = 0; block_index < blocks_per_word; ++block_index) {
			if (pos >= 4096) {
				break;
			}
			unsigned int state = (unsigned int) blocks[pos];
			word |= state << (bits_per_block * block_index);
			++pos;
		}
		put_unsigned_int_le(word, &stream);
	}
	put_signed_var_int(palette_length, &stream);
	for (int i = 0; i < palette_length; ++i) {
		put_signed_var_int(palette[i], &stream);
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
	binary_stream_t result = c_block_storage_network_serialize(blocks, palette, palette_length);
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
