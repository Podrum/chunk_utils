#include <Python.h>
#include <math.h>

typedef struct {
	char *buffer;
	int size;
} pack_t;

pack_t c_block_storage_serialize_blocks(int blocks[], int palette_length) {
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
	pack_t out;
	out.buffer = result;
	out.size = size;
	return out;
}

static PyObject *block_storage_serialize_blocks(PyObject *self, PyObject *args)
{
	PyObject *blocks_obj;
	PyObject *long_obj;
	int palette_length;
	if (!PyArg_ParseTuple(args, "Oi", &blocks_obj, &palette_length)) {
		return NULL;
	}
        int blocks[4096];
        int i;
        for (i = 0; i < 4096; ++i) {
		long_obj = PyList_GetItem(blocks_obj, i);
		blocks[i] = PyLong_AsLong(long_obj) & 0xffffff;
	}
	pack_t result = c_block_storage_serialize_blocks(blocks, palette_length);
	return PyBytes_FromStringAndSize(result.buffer, result.size);
}

static PyMethodDef myMethods[] = {
	{
		"block_storage_serialize_blocks",
		block_storage_serialize_blocks,
		METH_VARARGS,
		"Serialises blocks."
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
