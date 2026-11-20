#include "medusa_image/common.h"
#include "medusa_image/handler.h"

void medusa_image_init_data_buffer(asset_image* asset, void* buffer, int size){
	asset->data = buffer;
	asset->data_size = size;
}

void medusa_image_init_palette_buffer(asset_image* asset, void* buffer, int size){
	asset->palette = buffer;
	asset->palette_size = size;
}

void medusa_image_init_asset(asset_image* asset, int width, int height, int bits_per_pixel){
	asset->meta.width = width;
	asset->meta.height = height;
	asset->meta.bits_per_pixel = bits_per_pixel;
	asset->meta.reserved = 0;
	asset->meta.palette_entries = bits_per_pixel < 16 ? (1 << bits_per_pixel) : 0;

	medusa_image_update_effective_sizes(asset);

	medusa_image_init_data_buffer(asset, 0, 0);
	medusa_image_init_palette_buffer(asset, 0, 0);
}

int medusa_image_assign_buffers(asset_image* asset, void* block){
	if(!block){ return asset->effective_data_size + asset->effective_palette_size; }

	uint8_t* u8_block = block;

	medusa_image_init_data_buffer(asset, u8_block, asset->effective_data_size);
	u8_block += asset->effective_data_size;

	if(asset->effective_palette_size){
		medusa_image_init_palette_buffer(asset, u8_block, asset->effective_palette_size);
	} else{
		medusa_image_init_palette_buffer(asset, 0, 0);
	}

	return 0;
}

int medusa_image_create_asset(int width, int height, int bits_per_pixel, void* block){
	if(!block){
		asset_image dummy = {0};
		medusa_image_init_asset(&dummy, width, height, bits_per_pixel);
		return sizeof(asset_image) + dummy.effective_data_size + dummy.effective_palette_size;
	}

	asset_image* asset = (asset_image*)block;
	*asset = (asset_image){0};

	medusa_image_init_asset(asset, width, height, bits_per_pixel);

	uint8_t* u8_block = (uint8_t*)block;
	u8_block += sizeof(asset_image);

	medusa_image_assign_buffers(asset, u8_block);

	medusa_image_handler_clear(asset);
	medusa_image_handler_load_default_palette(asset);

	return 0;
}
