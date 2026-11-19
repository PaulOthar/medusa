#include "../../../include/medusa_image/common.h"

void medusa_image_init_asset(asset_image* asset, int width, int height, int bits_per_pixel){
	int data_size = (width * height * bits_per_pixel) / 8;
	int palette_size = bits_per_pixel > 16 ? 0 : (1 << bits_per_pixel);

	asset->data = 0; asset->palette = 0;
	asset->data_size = data_size;
	asset->effective_data_size = asset->data_size;
	asset->palette_size = palette_size * 4;
	asset->effective_palette_size = asset->palette_size;

	asset->meta.width = width;
	asset->meta.height = height;
	asset->meta.bits_per_pixel = bits_per_pixel;
	asset->meta.reserved = 0;
	asset->meta.palette_entries = 0;
}

int medusa_image_create_asset(int width, int height, int bits_per_pixel, void* block){
	asset_image dummy = {0};
	medusa_image_init_asset(&dummy, width, height, bits_per_pixel);

	int buffer_size = sizeof(asset_image) + dummy.data_size + dummy.palette_size;
	if(!block){ return buffer_size; }

	uint8_t* buffer = (uint8_t*)block;

	asset_image* result = (asset_image*)buffer; result[0] = dummy; buffer += sizeof(asset_image);
	result->data = buffer; buffer += dummy.data_size;
	result->palette = dummy.palette_size ? buffer : 0;

	return buffer_size;
}
