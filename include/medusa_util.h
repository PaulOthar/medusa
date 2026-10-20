#ifndef MEDUSA_UTIL
#define MEDUSA_UTIL

void medusa_util_copy_bits(void* src, void* dest, int offset, int size);

void medusa_util_push_bit(void* dest, int bit, int offset);

#endif
