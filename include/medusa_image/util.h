#ifndef MEDUSA_IMAGE_UTIL
#define MEDUSA_IMAGE_UTIL

#include "common.h"
#include "io.h"
#include "medusa_util.h"

//---------------------------------------------------------------------------------------------------
// Pixel manipulation
//---------------------------------------------------------------------------------------------------

uint32_t medusa_image_util_pixel_to_greyscale(uint32_t color);

//---------------------------------------------------------------------------------------------------
// Asset manipulation
//---------------------------------------------------------------------------------------------------

int medusa_image_util_format_placeholder_palette(asset_image* asset, void* buffer);

asset_image* medusa_image_util_asset_read_alloc(void* source, medusa_image_io reader, medusa_util_alloc alloc);
asset_image* medusa_image_util_asset_create_alloc(int width, int height, int bits_per_pixel, medusa_util_alloc alloc);

#endif
