#include "../../../include/medusa_image/handler.h"
#include "../../../include/medusa_image/io.h"
#include "../../../include/medusa_image/io.h"
#include "../../../include/medusa_image/util.h"

int medusa_image_util_format_placeholder_palette(asset_image* asset, void* buffer){
	int bpp = asset->meta.bits_per_pixel;
	int size = bpp < 16 ? ((1 << bpp) * 4) : 0;

	if(!buffer){
		return size;
	}

	asset->palette = buffer;

	int prev_palette_size = asset->palette_size;
	int prev_effective_palette_size = asset->effective_palette_size;

	asset->palette_size = size;
	asset->effective_palette_size = size;
	medusa_image_handler_load_default_palette(asset);
	asset->palette_size = prev_palette_size;
	asset->effective_palette_size = prev_effective_palette_size;

	return 0;
}

asset_image* medusa_image_util_asset_read_alloc(void* source, medusa_image_io reader, medusa_util_alloc alloc){
	void* block = alloc(medusa_image_io_asset_read(source, reader, 0));
	if(!block){ return 0; }

	medusa_image_io_asset_read(source, reader, block);
	return block;
}

asset_image* medusa_image_util_asset_create_alloc(int width, int height, int bits_per_pixel, medusa_util_alloc alloc){
	void* block = alloc(medusa_image_create_asset(width, height, bits_per_pixel, 0));
	if(!block){ return 0; }

	medusa_image_create_asset(width, height, bits_per_pixel, block);
	return block;
}
