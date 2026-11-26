#ifndef MEDUSA_IMAGE_IO
#define MEDUSA_IMAGE_IO

#include <stdio.h>

#include "common.h"

typedef int (*medusa_image_io)(asset_image*, void*);

int medusa_image_io_read_asset(void* source, medusa_image_io reader, void* block);

//---------------------------------------------------------------------------------------------------
// Bitmap(.bmp)
//---------------------------------------------------------------------------------------------------

int medusa_image_io_read_bmp(asset_image* asset, void* data);
int medusa_image_io_write_bmp(asset_image* asset, void* data);
int medusa_image_io_read_file_bmp(asset_image* asset, FILE* file);
int medusa_image_io_write_file_bmp(asset_image* asset, FILE* file);

//---------------------------------------------------------------------------------------------------
// Raw image(.ri)
//---------------------------------------------------------------------------------------------------

int medusa_image_io_read_ri(asset_image* asset, void* data);
int medusa_image_io_write_ri(asset_image* asset, void* data);
int medusa_image_io_read_file_ri(asset_image* asset, FILE* file);
int medusa_image_io_write_file_ri(asset_image* asset, FILE* file);

#endif
