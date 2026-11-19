#include "medusa_util.h"

#include <string.h>

#include "medusa_image/common.h"
#include "medusa_image/handler.h"
#include "medusa_image/util.h"

//Rule for implementation: Always assume that dest is src

int medusa_image_handler_greyscale(asset_image* src, asset_image* dest){
	int code = medusa_image_validate_equivalent(src, dest);
	if(code){ return code; }

	//Up to here, they are equivalent (albeit, possibly not equal)
	dest->meta = src->meta;

	if(src->meta.bits_per_pixel < 16){
		uint32_t* palette_src = src->palette;
		uint32_t* palette_dest = dest->palette;
		for(int i = 0; i < src->meta.palette_entries; i++){
			uint32_t color = palette_src[i];
			color = medusa_image_util_pixel_to_greyscale(color);
			palette_dest[i] = color;
		}
		if(src->data == dest->data){ return 0; }//If they are equal, then they already are copied over
		memcpy(dest->data, src->data, src->effective_data_size);
		return 0;
	}

	medusa_image_get_function get_pixel = medusa_image_function_get_pixel(src->meta.bits_per_pixel);

	int image_size = medusa_image_calc_image_size(src);
	for(int i = 0; i < image_size; i++){
		uint32_t color = get_pixel(src, i);
		color = medusa_image_util_pixel_to_greyscale(color);
		medusa_image_set_pixel(dest, i, color);
	}

	return 0;
}

//---------------------------------------------------------------------------------------------------
// Vertical flip
//---------------------------------------------------------------------------------------------------

void _flip_vertical_bytes(asset_image* src, asset_image* dest){
	int width = src->meta.width;
	int height = src->meta.height;

	int line_bytes = (width * src->meta.bits_per_pixel) / 8;

	uint8_t* src_ptr = src->data;
	uint8_t* dest_ptr = dest->data;

	int index_top = 0;
	int index_bot = (height - 1) * line_bytes;

	for(; index_top < index_bot;){
		for(int i = 0; i < line_bytes; i++){
			int top = src_ptr[i + index_top];
			int bot = src_ptr[i + index_bot];
			//Looks stupid, but remember, it may be the same buffer
			dest_ptr[i + index_top] = bot;
			dest_ptr[i + index_bot] = top;
		}
		index_top += line_bytes;
		index_bot -= line_bytes;
	}
}

void _flip_vertical_bits(asset_image* src, asset_image* dest){
	int width = src->meta.width;
	int height = src->meta.height;

	int line_bits = width * src->meta.bits_per_pixel;
	int line_bytes = (line_bits + 7) / 8;

	void* src_ptr = src->data;
	void* dest_ptr = dest->data;

	int index_top = 0;
	int index_bot = (height - 1) * line_bits;

	uint8_t top_buffer[line_bytes];
	uint8_t bot_buffer[line_bytes];

	for(; index_top < index_bot;){
		medusa_util_copy_bits_new(src_ptr, index_top, top_buffer, 0, line_bits);
		medusa_util_copy_bits_new(src_ptr, index_bot, bot_buffer, 0, line_bits);

		medusa_util_copy_bits_new(bot_buffer, 0, dest_ptr, index_top, line_bits);
		medusa_util_copy_bits_new(top_buffer, 0, dest_ptr, index_bot, line_bits);

		index_top += line_bits;
		index_bot -= line_bits;
	}
}

int medusa_image_handler_flip_vertical(asset_image* src, asset_image* dest){
	int code = medusa_image_validate_equivalent(src, dest);
	if(code & IMAGE_COMMON_CODE_MISSING_DATA){ return code; }//If a data error was thrown

	dest->meta = src->meta;

	if(src->meta.bits_per_pixel >= 8){ _flip_vertical_bytes(src, dest); }
	else{ _flip_vertical_bits(src, dest); }

	return code;
}

//---------------------------------------------------------------------------------------------------
// Other
//---------------------------------------------------------------------------------------------------

int medusa_image_handler_stack(asset_image* src, asset_image* dest, int x_off, int y_off){
	medusa_image_get_function get_func = medusa_image_function_get_pixel(src->meta.bits_per_pixel);
	medusa_image_set_function set_func = medusa_image_function_set_pixel(dest->meta.bits_per_pixel);

	int bpp = dest->meta.bits_per_pixel;
	if(bpp < 16){//If in indexed mode, only allow same bittage operations
		if(bpp != src->meta.bits_per_pixel){ return IMAGE_COMMON_CODE_GENERIC_ERROR; }
		get_func = medusa_image_function_get_palette_index(bpp);
		set_func = medusa_image_function_set_palette_index(bpp);
	}

	int src_width = src->meta.width, src_height = src->meta.height;
	int dest_width = dest->meta.width, dest_height = dest->meta.height;
	int dest_offset = x_off + (y_off * dest_width);

	int dw_m_sw = dest_width - src_width;
	int dh_m_dh = dest_height - src_height;

	int minx = x_off < 0 ? (-x_off) : 0;
	int maxx = x_off > dw_m_sw ? src_width - (x_off - dw_m_sw) : src_width;

	int miny = y_off < 0 ? (-y_off) : 0;
	int maxy = y_off > dh_m_dh ? src_height - (y_off - dh_m_dh) : src_height;

	int src_ptr = minx + (miny * src_width);
	int dest_ptr = minx + (miny * dest_width) + dest_offset;

	int initial_src_ptr = src_ptr;
	int initial_dest_ptr = dest_ptr;

	for(int i = miny; i < maxy; i++){
		for(int l = minx; l < maxx; l++, dest_ptr++, src_ptr++){
			uint32_t value = get_func(src, src_ptr);
			if(!value){ continue; }
			set_func(dest, dest_ptr, value);
		}
		initial_dest_ptr += dest_width; dest_ptr = initial_dest_ptr;
		initial_src_ptr += src_width; src_ptr = initial_src_ptr;
	}

	return 0;
}
