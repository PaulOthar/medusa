#ifndef MEDUSA_UTIL
#define MEDUSA_UTIL

#include <stddef.h>
#include <stdio.h>

typedef void* (*medusa_util_alloc)(size_t);

//---------------------------------------------------------------------------------------------------
// Bits [Read & Write]
//---------------------------------------------------------------------------------------------------

void medusa_util_copy_bits(void* src, void* dest, int offset, int size);
void medusa_util_copy_bits_new(void* src, int src_off, void* dest, int dest_off, int size);

void medusa_util_push_bit(void* dest, int bit, int offset);

void medusa_util_set_byte(void* buffer, int offset, int data);

void medusa_util_switch_bit_position(void* data, int index, int bits_length);
void medusa_util_switch_bits_positions(void* data, int length, int bits_length);

//---------------------------------------------------------------------------------------------------
// Bytes [Read & Write]
//---------------------------------------------------------------------------------------------------

typedef void* (*medusa_util_rw)(void* ptr, int size, int length, void* file, int offset);

void medusa_util_seek_and_read_memory(void* ptr, int size, int length, void* block, int offset);
void medusa_util_seek_and_write_memory(void* ptr, int size, int length, void* block, int offset);

void medusa_util_seek_and_read_file(void* ptr, int size, int length, FILE* file, int offset);
void medusa_util_seek_and_write_file(void* ptr, int size, int length, FILE* file, int offset);

#endif
