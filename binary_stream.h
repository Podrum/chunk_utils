#ifndef BINARY_STREAM_H
#define BINARY_STREAM_H

typedef struct {
	char *buffer;
	int size;
	int offset;
} binary_stream_t;

char *get(int count, binary_stream_t *stream);
char *get_remaining(binary_stream_t *stream);
unsigned char get_unsigned_byte(binary_stream_t *stream);
unsigned short get_unsigned_short_le(binary_stream_t *stream);
unsigned short get_unsigned_short_be(binary_stream_t *stream);
unsigned int get_unsigned_triad_le(binary_stream_t *stream);
unsigned int get_unsigned_triad_be(binary_stream_t *stream);
unsigned int get_unsigned_int_le(binary_stream_t *stream);
unsigned int get_unsigned_int_be(binary_stream_t *stream);
unsigned long int get_unsigned_long_le(binary_stream_t *stream);
unsigned long int get_unsigned_long_be(binary_stream_t *stream);
unsigned int get_var_int(binary_stream_t *stream);
int get_signed_var_int(binary_stream_t *stream);
unsigned long int get_var_long(binary_stream_t *stream);
long int get_signed_var_long(binary_stream_t *stream);
void put_unsigned_byte(unsigned char value, binary_stream_t *stream);
void put_unsigned_short_le(unsigned short value, binary_stream_t *stream);
void put_unsigned_short_be(unsigned short value, binary_stream_t *stream);
void put_unsigned_triad_le(unsigned int value, binary_stream_t *stream);
void put_unsigned_triad_be(unsigned int value, binary_stream_t *stream);
void put_unsigned_int_le(unsigned int value, binary_stream_t *stream);
void put_unsigned_int_be(unsigned int value, binary_stream_t *stream);
void put_unsigned_long_le(unsigned long int value, binary_stream_t *stream);
void put_unsigned_long_be(unsigned long int value, binary_stream_t *stream);
void put_var_int(unsigned int value, binary_stream_t *stream);
void put_signed_var_int(int value, binary_stream_t *stream);
void put_var_long(unsigned long int value, binary_stream_t *stream);
void put_signed_var_long(long int value, binary_stream_t *stream);

#endif
