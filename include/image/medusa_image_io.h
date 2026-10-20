#ifndef IMAGE_IO
#define IMAGE_IO

#include <stdio.h>

#include "medusa_image_common.h"

int medusa_image_io_read_bmp(asset_image* asset, void* data);

int medusa_image_io_write_bmp(asset_image* asset, void* data);

int medusa_image_io_read_bmp_file(asset_image* asset, FILE* file);

int medusa_image_io_write_bmp_file(asset_image* asset, FILE* file);

#endif
