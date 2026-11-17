#include "image/medusa_image_handler.h"
#include "image/medusa_image_util.h"
#include "image/medusa_image.h"

#include <string.h>

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
		memcpy(dest->data, src->data, medusa_image_calc_data_size(src));
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

int medusa_image_handler_stack(asset_image* src, asset_image* dest){
	int size = medusa_image_calc_image_size(dest);
	if(medusa_image_calc_image_size(src) != size){ return IMAGE_COMMON_CODE_INVALID_SIZE; }

	medusa_image_get_function get_func = medusa_image_function_get_pixel(src->meta.bits_per_pixel);
	medusa_image_set_function set_func = medusa_image_function_set_pixel(dest->meta.bits_per_pixel);

	int bpp = dest->meta.bits_per_pixel;
	if(bpp < 16){//If in indexed mode, only allow same bittage operations
		if(bpp != src->meta.bits_per_pixel){ return IMAGE_COMMON_CODE_GENERIC_ERROR; }
		get_func = medusa_image_function_get_palette_index(bpp);
		set_func = medusa_image_function_set_palette_index(bpp);
	}

	for(int i = 0; i < size; i++){
		uint32_t value = get_func(src, i);
		if(!value){ continue; }
		set_func(dest, i, value);
	}

	return 0;
}
