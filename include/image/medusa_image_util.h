#ifndef MEDUSA_IMAGE_UTIL
#define MEDUSA_IMAGE_UTIL

#include "medusa_image.h"

//---------------------------------------------------------------------------------------------------
// Pixel manipulation
//---------------------------------------------------------------------------------------------------

uint32_t medusa_image_util_pixel_to_greyscale(uint32_t color);

//---------------------------------------------------------------------------------------------------
// Asset manipulation
//---------------------------------------------------------------------------------------------------

asset_image* medusa_image_util_asset_read_malloc(void* source, int format);
asset_image* medusa_image_util_asset_create_malloc(int width, int height, int bits_per_pixel);

#endif
