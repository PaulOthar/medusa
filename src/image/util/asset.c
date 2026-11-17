#include "image/medusa_image_util.h"
#include "image/medusa_image_io.h"
#include <stdlib.h>

asset_image* medusa_image_util_asset_read_malloc(void* source, int format){
	void* block = malloc(medusa_image_io_asset_read(source, format, 0));
	if(!block){ return 0; }

	medusa_image_io_asset_read(source, format, block);
	return block;
}

asset_image* medusa_image_util_asset_create_malloc(int width, int height, int bits_per_pixel){
	void* block = malloc(medusa_image_create_asset(width, height, bits_per_pixel, 0));
	if(!block){ return 0; }

	medusa_image_create_asset(width, height, bits_per_pixel, block);
	return block;
}
