#include "image/common.h"

int image_common_calculate_data_size(asset_image* asset){
	return (asset->meta.width * asset->meta.height * asset->meta.bits_per_pixel / 8);
}

int image_common_calculate_palette_size(asset_image* asset){
	return asset->meta.palette_entries * 4;;
}

int image_common_validate_buffers(asset_image* asset){
	int code = 0;

	int data_size = image_common_calculate_data_size(asset);
	if(!asset->data && data_size){
		code |= IMAGE_COMMON_CODE_MISSING_DATA;
		asset->data_size = data_size;
	} else if(asset->data_size < data_size){
		code |= IMAGE_COMMON_CODE_INVALID_DATA;
	}

	int palette_size = image_common_calculate_palette_size(asset);
	if(!asset->palette && palette_size){
		code |= IMAGE_COMMON_CODE_MISSING_PALT;
		asset->palette_size = palette_size;
	} else if(asset->palette_size < palette_size){
		code |= IMAGE_COMMON_CODE_INVALID_PALT;
	}

	return code;
}

static uint32_t default_colors[] = {
	0xff00ff, 0x010101, 0x818181, 0xffffff,
	0xff0000, 0xaa0000, 0x550000, 0x2a0000,
	0x00ff00, 0x00aa00, 0x005500, 0x002a00,
	0x00ffff, 0x00aaaa, 0x005555, 0x002a2a
};

void image_common_load_default_palette(asset_image* asset, void* buffer){
	uint32_t* dest_word = buffer;

	switch(asset->meta.bits_per_pixel){
	case 1:
		dest_word[0] = 0;
		dest_word[1] = -1;
		break;
	case 4:
		for(int i = 0; i < 16; i++){ dest_word[i] = default_colors[i]; }
		break;
	case 8:{//Pretty much grey scale
		for(int i = 0; i < 256; i++){ dest_word[i] = i * 0x010101; }
		break;
	}
	default:
		break;
	}
}
