#include "medusa_image/common.h"

medusa_image_get_function medusa_image_function_get_palette_index(int bpp){
	switch(bpp){
		case 8: return medusa_image_get_palette_index_8bpp;
		case 4: return medusa_image_get_palette_index_4bpp;
		case 1: return medusa_image_get_palette_index_1bpp;
		default: return 0;
	}
}

uint32_t medusa_image_get_palette_index(asset_image* asset, int pos){
	switch(asset->meta.bits_per_pixel){
		case 8: return medusa_image_get_palette_index_8bpp(asset, pos);
		case 4: return medusa_image_get_palette_index_4bpp(asset, pos);
		case 1: return medusa_image_get_palette_index_1bpp(asset, pos);
		default: return -1;
	}
}

uint32_t medusa_image_get_palette_index_8bpp(asset_image* asset, int pos){
	return ((uint8_t*)asset->data)[pos];
}

uint32_t medusa_image_get_palette_index_4bpp(asset_image* asset, int pos){
	uint8_t byte = ((uint8_t*)asset->data)[pos >> 1];
	byte >>= 4 * (pos & 1); byte &= 0xf;
	return byte;
}

uint32_t medusa_image_get_palette_index_1bpp(asset_image* asset, int pos){
	uint8_t byte = ((uint8_t*)asset->data)[pos >> 3];
	byte >>= pos & 7; byte &= 1;
	return byte;
}

//---------------------------------------------------------------------------------------------------

medusa_image_set_function medusa_image_function_set_palette_index(int bpp){
	switch(bpp){
		case 8: return medusa_image_set_palette_index_8bpp;
		case 4: return medusa_image_set_palette_index_4bpp;
		case 1: return medusa_image_set_palette_index_1bpp;
		default: return 0;
	}
}

uint32_t medusa_image_set_palette_index(asset_image* asset, int pos, uint32_t index){
	switch(asset->meta.bits_per_pixel){
		case 8: return medusa_image_set_palette_index_8bpp(asset, pos, index);
		case 4: return medusa_image_set_palette_index_4bpp(asset, pos, index);
		case 1: return medusa_image_set_palette_index_1bpp(asset, pos, index);
		default: return -1;
	}
}

uint32_t medusa_image_set_palette_index_8bpp(asset_image* asset, int pos, uint32_t index){
	uint8_t* addr = ((uint8_t*)asset->data) + pos;
	uint32_t prev = addr[0];
	index &= 0xff;
	addr[0] = index;
	return prev;
}

uint32_t medusa_image_set_palette_index_4bpp(asset_image* asset, int pos, uint32_t index){
	uint8_t* addr = ((uint8_t*)asset->data) + (pos >> 1);
	int shift = 4 * (pos & 1);
	uint32_t prev = (addr[0] >> shift) & 0xf;
	index &= 0xf;
	addr[0] &= ~(0xf << shift);//clears out whatever it was
	addr[0] |= index << shift;//puts the new data
	return prev;
}

uint32_t medusa_image_set_palette_index_1bpp(asset_image* asset, int pos, uint32_t index){
	uint8_t* addr = ((uint8_t*)asset->data) + (pos >> 3);
	int shift = pos & 7;
	uint32_t prev = (addr[0] >> shift) & 1;
	index &= 1;
	addr[0] &= ~(1 << shift);//clears out whatever it was
	addr[0] |= index << shift;//puts the new data
	return prev;
}

//---------------------------------------------------------------------------------------------------

medusa_image_get_function medusa_image_function_get_pixel(int bpp){
	switch(bpp){
		case 32: return medusa_image_get_pixel_32bpp;
		case 24: return medusa_image_get_pixel_24bpp;
		case 8: return medusa_image_get_pixel_8bpp;
		case 4: return medusa_image_get_pixel_4bpp;
		case 1: return medusa_image_get_pixel_1bpp;
		default: return 0;
	}
}

uint32_t medusa_image_get_pixel(asset_image* asset, int pos){
	switch(asset->meta.bits_per_pixel){
		case 32: return medusa_image_get_pixel_32bpp(asset, pos);
		case 24: return medusa_image_get_pixel_24bpp(asset, pos);
		case 8: return medusa_image_get_pixel_8bpp(asset, pos);
		case 4: return medusa_image_get_pixel_4bpp(asset, pos);
		case 1: return medusa_image_get_pixel_1bpp(asset, pos);
		default: return -1;
	}
}

uint32_t medusa_image_get_pixel_32bpp(asset_image* asset, int pos){
	uint32_t* word_data = (uint32_t*)asset->data;
	return word_data[pos];
}

uint32_t medusa_image_get_pixel_24bpp(asset_image* asset, int pos){
	uint8_t* ptr = ((uint8_t*)asset->data) + (pos * 3);
	uint32_t result = ((uint32_t*)ptr)[0];
	return result & 0xffffff;
}

uint32_t medusa_image_get_pixel_8bpp(asset_image* asset, int pos){
	return ((uint32_t*)asset->palette)[medusa_image_get_palette_index_8bpp(asset, pos)];
}

uint32_t medusa_image_get_pixel_4bpp(asset_image* asset, int pos){
	return ((uint32_t*)asset->palette)[medusa_image_get_palette_index_4bpp(asset, pos)];
}

uint32_t medusa_image_get_pixel_1bpp(asset_image* asset, int pos){
	return ((uint32_t*)asset->palette)[medusa_image_get_palette_index_1bpp(asset, pos)];
}

//---------------------------------------------------------------------------------------------------

medusa_image_set_function medusa_image_function_set_pixel(int bpp){
	switch(bpp){
		case 32: return medusa_image_set_pixel_32bpp;
		case 24: return medusa_image_set_pixel_24bpp;
		default: return 0;
	}
}

uint32_t medusa_image_set_pixel(asset_image* asset, int pos, uint32_t color){
	switch(asset->meta.bits_per_pixel){
		case 32: return medusa_image_set_pixel_32bpp(asset, pos, color);
		case 24: return medusa_image_set_pixel_24bpp(asset, pos, color);
		default: return -1;
	}
}

uint32_t medusa_image_set_pixel_32bpp(asset_image* asset, int pos, uint32_t color){
	uint32_t* word_data = (uint32_t*)asset->data;
	uint32_t prev = word_data[pos];
	word_data[pos] = color;
	return prev;
}

uint32_t medusa_image_set_pixel_24bpp(asset_image* asset, int pos, uint32_t color){
	uint8_t* data_ptr = ((uint8_t*)asset->data) + (pos * 3);
	uint8_t* color_ptr = (uint8_t*)(&color);

	uint32_t prev = ((uint32_t*)data_ptr)[0] & 0xffffff;

	for(int i = 0; i < 3; i++){ data_ptr[i] = color_ptr[i]; }

	return prev;
}
