#ifndef MEDUSA_IMAGE_IO_GENERIC_IMPLEMENTATION
#define MEDUSA_IMAGE_IO_GENERIC_IMPLEMENTATION

#include "medusa_image/handler.h"
#include "medusa_image/common.h"
#include "medusa_util.h"

int _medusa_file_read_palette(asset_image* asset, void* file, medusa_util_rw reader, int file_offset, int code);

int _medusa_file_write_palette(asset_image* asset, void* file, medusa_util_rw writter, int file_offset, int code);

#endif
