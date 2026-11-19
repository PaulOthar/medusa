#include "medusa_util.h"
#include <string.h>
#include <stdint.h>

void medusa_util_seek_and_read_file(void* ptr, int size, int length, FILE* file, int offset){
	fseek(file, offset, SEEK_SET);
	fread(ptr, size, length, file);
}

void medusa_util_seek_and_write_file(void* ptr, int size, int length, FILE* file, int offset){
	fseek(file, offset, SEEK_SET);
	fwrite(ptr, size, length, file);
}

void medusa_util_seek_and_read_memory(void* ptr, int size, int length, void* block, int offset){
	block = ((uint8_t*)block) + offset;//fseek
	memcpy(ptr, block, size * length);//fread
}

void medusa_util_seek_and_write_memory(void* ptr, int size, int length, void* block, int offset){
	block = ((uint8_t*)block) + offset;//fseek
	memcpy(block, ptr, size * length);//fread
}
