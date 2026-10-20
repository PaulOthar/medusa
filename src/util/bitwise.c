#include "medusa_util.h"

#include <stdint.h>

union byte_assist{
	uint32_t raw;

	struct{
		uint32_t bit : 3;
		uint32_t byte : 29;
	}field;
};

void medusa_util_copy_bits(void* src, void* dest, int offset, int size){
	uint8_t* _src = src;
	uint8_t* _dest = dest;

	union byte_assist oass = {.raw = offset};

	uint32_t usize = size;
	for(union byte_assist wass = {.raw = 0}; wass.raw < usize; wass.raw++, oass.raw++){
		uint8_t bit = (_src[oass.field.byte] >> oass.field.bit) & 1;

		uint8_t dbyte = _dest[wass.field.byte];
		dbyte &= ~(1 << wass.field.bit);//clear that bit
		dbyte |= bit << wass.field.bit;//set that bit
		_dest[wass.field.byte] = dbyte;
	}
}

void medusa_util_push_bit(void* dest, int bit, int offset){
	uint8_t* _dest = dest;
	union byte_assist oass = {.raw = offset};

	uint8_t dbyte = _dest[oass.field.byte];
	dbyte &= ~(1 << oass.field.bit);//clear that bit
	dbyte |= bit << oass.field.bit;//set that bit
	_dest[oass.field.byte] = dbyte;
}
