#ifndef BINARY_STREAM_H
#define BINARY_STREAM_H

typedef struct {
	char *buffer;
	int size;
	int offset;
} binary_stream_t;

#endif
