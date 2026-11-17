#include "image/medusa_image.h"

int medusa_image_calc_data_size(asset_image* asset){
	return (asset->meta.width * asset->meta.height * asset->meta.bits_per_pixel / 8);
}

int medusa_image_calc_palette_size(asset_image* asset){
	return asset->meta.palette_entries * 4;
}

int medusa_image_calc_image_size(asset_image* asset){
	return asset->meta.width * asset->meta.height;
}

int medusa_image_validate_buffers(asset_image* asset){
	return medusa_image_validate_equivalent(asset, asset);
}

int medusa_image_validate_equivalent(asset_image* ref, asset_image* eq){
	int code = 0;

	int data_size = medusa_image_calc_data_size(ref);
	if(!eq->data && data_size){
		code |= IMAGE_COMMON_CODE_MISSING_DATA;
		eq->data_size = data_size;
	} else if(eq->data_size < data_size){
		code |= IMAGE_COMMON_CODE_INVALID_DATA;
	}

	int palette_size = medusa_image_calc_palette_size(ref);
	if(!eq->palette && palette_size){
		code |= IMAGE_COMMON_CODE_MISSING_PALT;
		eq->palette_size = palette_size;
	} else if(eq->palette_size < palette_size){
		code |= IMAGE_COMMON_CODE_INVALID_PALT;
	}

	return code;
}
