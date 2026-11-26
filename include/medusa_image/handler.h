#ifndef MEDUSA_IMAGE_HANDLER
#define MEDUSA_IMAGE_HANDLER

#include "common.h"

//---------------------------------------------------------------------------------------------------
// Initialization
//---------------------------------------------------------------------------------------------------

int medusa_image_handler_clear(asset_image* asset);

int medusa_image_handler_write_default_palette(int entries, int size, void* buffer);

int medusa_image_handler_load_default_palette(asset_image* asset);

int medusa_image_handler_get_palette_colors(asset_image* asset, int pos, uint32_t* buffer, int size);

int medusa_image_handler_set_palette_colors(asset_image* asset, int pos, uint32_t* colors, int size);

//---------------------------------------------------------------------------------------------------
// Manipulation
//---------------------------------------------------------------------------------------------------

int medusa_image_handler_greyscale(asset_image* src, asset_image* dest);

int medusa_image_handler_flip_vertical(asset_image* src, asset_image* dest);

int medusa_image_handler_flip_horizontal(asset_image* src, asset_image* dest);

int medusa_image_handler_crop(asset_image* src, asset_image* dest, int x_src, int y_src, int width, int height, int x_dest, int y_dest);

//---------------------------------------------------------------------------------------------------
// Dither
//---------------------------------------------------------------------------------------------------

void medusa_image_handler_dither_stupid(asset_image* src, asset_image* dest);
void medusa_image_handler_dither_gridded(asset_image* src, asset_image* dest);
void medusa_image_handler_dither_floyd_steinberg(asset_image* src, asset_image* dest);
void medusa_image_handler_dither_atkinson(asset_image* src, asset_image* dest);
void medusa_image_handler_dither_jarvis_judice_ninke(asset_image* asset, asset_image* dest);

#endif
