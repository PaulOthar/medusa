#ifndef IMAGE_COMMON
#define IMAGE_COMMON

#include <stdint.h>

typedef struct /*metadata_image*/{
	uint16_t width;
	uint16_t height;
	uint8_t bits_per_pixel;

	uint8_t reserved;
	uint16_t palette_entries;
}metadata_image;

typedef struct /*asset_image*/{
	metadata_image meta;

	void* palette; int palette_size;
	void* data; int data_size;
}asset_image;

enum image_io_code{
	IMAGE_COMMON_CODE_MISSING_DATA	=	2,
	IMAGE_COMMON_CODE_INVALID_DATA	=	3,
	IMAGE_COMMON_CODE_MISSING_PALT	=	4,
	IMAGE_COMMON_CODE_INVALID_PALT	=	5,
};

int image_common_calculate_data_size(asset_image* asset);

int image_common_calculate_palette_size(asset_image* asset);

int image_common_calculate_image_size(asset_image* asset);

int image_common_validate_buffers(asset_image* asset);

void image_common_load_default_palette(asset_image* asset, void* buffer);

uint32_t image_common_get_pixel(asset_image* asset, int index);

//---------------------------------------------------------------------------------------------------

void image_common_to_greyscale(asset_image* asset);

#endif
