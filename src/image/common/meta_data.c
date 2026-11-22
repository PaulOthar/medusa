#include "medusa_image/common.h"

static int _calc_data_size(asset_image* asset){
	int bits = asset->meta.width * asset->meta.height * asset->meta.bits_per_pixel;
	bits = (bits + 7) / 8;
	return bits;
}

static int _calc_palette_size(asset_image* asset){
	return asset->meta.palette_entries * 4;
}

void medusa_image_update_effective_sizes(asset_image* asset){
	asset->effective_data_size = _calc_data_size(asset);
	asset->effective_palette_size = _calc_palette_size(asset);
}

int medusa_image_validate_buffers(asset_image* asset){
	return medusa_image_validate_equivalent(asset, asset);
}

int medusa_image_validate_equivalent(asset_image* ref, asset_image* eq){
	int code = 0;

	int data_size = ref->effective_data_size;//medusa_image_calc_data_size(ref);
	if(!eq->data && data_size){//if it does'nt have a buffer, but declared a size
		code |= IMAGE_COMMON_CODE_MISSING_DATA;
	} else if(eq->data_size < data_size){//if the declared size is not enough to store
		code |= IMAGE_COMMON_CODE_INVALID_DATA;
	}

	int palette_size = ref->effective_palette_size;//medusa_image_calc_palette_size(ref);
	if(!eq->palette && palette_size){//if it does'nt have a buffer, but declared a size
		code |= IMAGE_COMMON_CODE_MISSING_PALT;
	} else if(eq->palette_size < palette_size){//if the declared size is not enough to store
		code |= IMAGE_COMMON_CODE_INVALID_PALT;
	}

	return code;
}

int medusa_image_calc_image_size(asset_image* asset){
	return asset->meta.width * asset->meta.height;
}
