#include "medusa_util.h"

#include <stdint.h>

union byte_assist{
	uint32_t raw;

	struct{
		uint32_t bit : 3;
		uint32_t byte : 29;
	}field;
};

void medusa_util_copy_bits_new(void* src, int src_off, void* dest, int dest_off, int size){
	uint8_t* _src = src;
	uint8_t* _dest = dest;

	union byte_assist s_ass = {.raw = src_off};//source assist
	union byte_assist d_ass = {.raw = dest_off};//destination assist

	for(int i = 0; i < size; i++, s_ass.raw++, d_ass.raw++){
		uint8_t bit = (_src[s_ass.field.byte] >> s_ass.field.bit) & 1;

		uint8_t dbyte = _dest[d_ass.field.byte];
		dbyte &= ~(1 << d_ass.field.bit);//clear that bit
		dbyte |= bit << d_ass.field.bit;//set that bit
		_dest[d_ass.field.byte] = dbyte;//puts the byte back
	}
}

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
}//Deprecated

void medusa_util_push_bit(void* dest, int bit, int offset){
	uint8_t* _dest = dest;
	union byte_assist oass = {.raw = offset};

	uint8_t dbyte = _dest[oass.field.byte];
	dbyte &= ~(1 << oass.field.bit);//clear that bit
	dbyte |= bit << oass.field.bit;//set that bit
	_dest[oass.field.byte] = dbyte;
}

void medusa_util_switch_bit_position(void* data, int index, int bits_length){
	uint8_t* ptr = data;
	int mask = (1 << bits_length) - 1;

	int switch_buffer = 0;
	int byte = ptr[index];

	for(int l = 0; l < 8; l += bits_length){
		switch_buffer <<= bits_length;
		switch_buffer |= byte & mask;
		byte >>= bits_length;
	}

	ptr[index] = switch_buffer;
}

void medusa_util_switch_bits_positions(void* data, int length, int bits_length){
	uint8_t* ptr = data;

	int mask = (1 << bits_length) - 1;

	for(int i = 0; i < length; i++){
		int switch_buffer = 0;
		int byte = ptr[i];

		for(int l = 0; l < 8; l += bits_length){
			switch_buffer <<= bits_length;
			switch_buffer |= byte & mask;
			byte >>= bits_length;
		}

		ptr[i] = switch_buffer;
	}
}
