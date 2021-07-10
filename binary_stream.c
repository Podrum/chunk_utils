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
