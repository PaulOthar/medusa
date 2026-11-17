#ifndef MEDUSA_IMAGE_IO
#define MEDUSA_IMAGE_IO

#include <stdio.h>

#include "medusa_image.h"

typedef int (*medusa_image_io)(asset_image*, void*);

enum medusa_image_io_format{
	IMAGE_FORMAT_BMP_MEMORY = 1,
	IMAGE_FORMAT_BMP_FILE = 2
};

int medusa_image_io_asset_read(void* source, int format, void* block);

int medusa_image_io_read_bmp(asset_image* asset, void* data);
int medusa_image_io_write_bmp(asset_image* asset, void* data);
int medusa_image_io_read_bmp_file(asset_image* asset, FILE* file);
int medusa_image_io_write_bmp_file(asset_image* asset, FILE* file);

#endif
