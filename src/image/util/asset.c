#include "medusa_image/handler.h"
#include "medusa_image/io.h"
#include "medusa_image/io.h"
#include "medusa_image/util.h"

int medusa_image_util_format_placeholder_palette(asset_image* asset, void* buffer){
	int bpp = asset->meta.bits_per_pixel;
	int size = bpp < 16 ? ((1 << bpp) * 4) : 0;

	if(!buffer){ return size; }

	asset->palette = buffer;

	medusa_image_handler_write_default_palette(size / 4, size, buffer);

	return 0;
}

asset_image* medusa_image_util_read_asset_alloc(void* source, medusa_image_io reader, medusa_util_alloc alloc){
	void* block = alloc(medusa_image_io_read_asset(source, reader, 0));
	if(!block){ return 0; }

	medusa_image_io_read_asset(source, reader, block);
	return block;
}

asset_image* medusa_image_util_create_asset_alloc(int width, int height, int bits_per_pixel, medusa_util_alloc alloc){
	void* block = alloc(medusa_image_create_asset(width, height, bits_per_pixel, 0));
	if(!block){ return 0; }

	medusa_image_create_asset(width, height, bits_per_pixel, block);
	return block;
}
