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

unsigned char get_unsigned_byte(binary_stream_t *stream) {
	unsigned char value = stream->buffer[stream->offset] & 0xff;
	++stream->offset;
	return value;
}

unsigned short get_unsigned_short_le(binary_stream_t *stream) {
	unsigned short value = stream->buffer[stream->offset] & 0xff;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 8;
	++stream->offset;
	return value;
}

unsigned short get_unsigned_short_be(binary_stream_t *stream) {
	unsigned short value = (stream->buffer[stream->offset] & 0xff) << 8;
	++stream->offset;
	value |= stream->buffer[stream->offset] & 0xff;
	++stream->offset;
	return value;
}

unsigned int get_unsigned_triad_le(binary_stream_t *stream) {
	unsigned short value = stream->buffer[stream->offset] & 0xff;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 8;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 16;
	++stream->offset;
	return value;
}

unsigned int get_unsigned_triad_be(binary_stream_t *stream) {
	unsigned short value = (stream->buffer[stream->offset] & 0xff) << 16;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 8;
	++stream->offset;
	value |= stream->buffer[stream->offset] & 0xff;
	++stream->offset;
	return value;
}

unsigned int get_unsigned_int_le(binary_stream_t *stream) {
	unsigned short value = stream->buffer[stream->offset] & 0xff;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 8;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 16;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 24;
	++stream->offset;
	return value;
}

unsigned int get_unsigned_int_be(binary_stream_t *stream) {
	unsigned short value = (stream->buffer[stream->offset] & 0xff) << 24;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 16;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 8;
	++stream->offset;
	value |= stream->buffer[stream->offset] & 0xff;
	++stream->offset;
	return value;
}

unsigned long int get_unsigned_long_le(binary_stream_t *stream) {
	unsigned short value = stream->buffer[stream->offset] & 0xff;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 8;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 16;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 24;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 32;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 40;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 48;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 56;
	++stream->offset;
	return value;
}

unsigned long int get_unsigned_long_be(binary_stream_t *stream) {
	unsigned short value = (stream->buffer[stream->offset] & 0xff) << 56;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 48;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 40;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 32;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 24;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 16;
	++stream->offset;
	value |= (stream->buffer[stream->offset] & 0xff) << 8;
	++stream->offset;
	value |= stream->buffer[stream->offset] & 0xff;
	++stream->offset;
	return value;
}

unsigned int get_var_int(binary_stream_t *stream) {
	int value = 0;
	for (int i = 0; i < 35; i += 7) {
		unsigned char to_read = get_unsigned_byte(stream);
		value |= ((to_read & 0x7f) << i);
		if ((to_read & 0x80) == 0) {
			return value;
		}
	}
}

int get_signed_var_int(binary_stream_t *stream) {
	unsigned int raw = get_var_int(stream);
	int temp = (((raw << 31) >> 31) ^ raw) >> 1;
	return temp ^ (raw & (1 << 31));
}

unsigned long int get_var_long(binary_stream_t *stream) {
	int value = 0;
	for (int i = 0; i < 70; i += 7) {
		unsigned char to_read = get_unsigned_byte(stream);
		value |= ((to_read & 0x7f) << i);
		if ((to_read & 0x80) == 0) {
			return value;
		}
	}
}

long int get_signed_var_long(binary_stream_t *stream) {
	unsigned long int raw = get_var_long(stream);
	long int temp = (((raw << 63) >> 63) ^ raw) >> 1;
	return temp ^ (raw & (1 << 63));
}

void put_unsigned_byte(unsigned char value, binary_stream_t *stream) {
	stream->buffer = realloc(stream->buffer, (stream->size + 1) * sizeof(char));
	stream->buffer[stream->size] = value & 0xff;
	++stream->size;
}

void put_unsigned_short_le(unsigned short value, binary_stream_t *stream) {
	stream->buffer = realloc(stream->buffer, (stream->size + 2) * sizeof(char));
	stream->buffer[stream->size] = value & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 8) & 0xff;
	++stream->size;
}


void put_unsigned_short_be(unsigned short value, binary_stream_t *stream) {
	stream->buffer = realloc(stream->buffer, (stream->size + 2) * sizeof(char));
	stream->buffer[stream->size] = (value >> 8) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = value & 0xff;
	++stream->size;
}

void put_unsigned_triad_le(unsigned int value, binary_stream_t *stream) {
	stream->buffer = realloc(stream->buffer, (stream->size + 3) * sizeof(char));
	stream->buffer[stream->size] = value & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 8) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 16) & 0xff;
	++stream->size;
}


void put_unsigned_triad_be(unsigned int value, binary_stream_t *stream) {
	stream->buffer = realloc(stream->buffer, (stream->size + 3) * sizeof(char));
	stream->buffer[stream->size] = (value >> 16) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 8) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = value & 0xff;
	++stream->size;
}

void put_unsigned_int_le(unsigned int value, binary_stream_t *stream) {
	stream->buffer = realloc(stream->buffer, (stream->size + 4) * sizeof(char));
	stream->buffer[stream->size] = value & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 8) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 16) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 24) & 0xff;
	++stream->size;
}


void put_unsigned_int_be(unsigned int value, binary_stream_t *stream) {
	stream->buffer = realloc(stream->buffer, (stream->size + 4) * sizeof(char));
	stream->buffer[stream->size] = (value >> 24) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 16) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 8) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = value & 0xff;
	++stream->size;
}

void put_unsigned_long_le(unsigned long int value, binary_stream_t *stream) {
	stream->buffer = realloc(stream->buffer, (stream->size + 8) * sizeof(char));
	stream->buffer[stream->size] = value & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 8) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 16) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 24) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 32) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 40) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 48) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 56) & 0xff;
	++stream->size;
}


void put_unsigned_long_be(unsigned long int value, binary_stream_t *stream) {
	stream->buffer = realloc(stream->buffer, (stream->size + 8) * sizeof(char));
	stream->buffer[stream->size] = (value >> 56) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 48) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 40) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 32) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 24) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 16) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = (value >> 8) & 0xff;
	++stream->size;
	stream->buffer[stream->size] = value & 0xff;
	++stream->size;
}

void put_var_int(unsigned int value, binary_stream_t *stream) {
	value &= 0xffffffff;
	for (int i = 0; i < 5; ++i) {
		unsigned char to_write = value & 0x7f;
		value >>= 7;
		stream->buffer = realloc(stream->buffer, (stream->size + 1) * sizeof(char));
		if (value != 0) {
			stream->buffer[stream->size] = (to_write | 0x80);
			++stream->size;
		} else {
			stream->buffer[stream->size] = to_write;
			++stream->size;
			break;
		}
	}
}

void put_signed_var_int(int value, binary_stream_t *stream) {
	put_var_int((value << 1) ^ (value >> 31), stream);
}

void put_var_long(unsigned long int value, binary_stream_t *stream) {
	for (int i = 0; i < 10; ++i) {
		unsigned char to_write = value & 0x7f;
		value >>= 7;
		stream->buffer = realloc(stream->buffer, (stream->size + 1) * sizeof(char));
		if (value != 0) {
			stream->buffer[stream->size] = (to_write | 0x80);
			++stream->size;
		} else {
			stream->buffer[stream->size] = to_write;
			++stream->size;
			break;
		}
	}
}

void put_signed_var_long(long int value, binary_stream_t *stream) {
	put_var_long((value << 1) ^ (value >> 63), stream);
}
