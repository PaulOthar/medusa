#ifndef MEDUSA_IMAGE_HANDLER
#define MEDUSA_IMAGE_HANDLER

#include "medusa_image.h"

//---------------------------------------------------------------------------------------------------
// Initialization
//---------------------------------------------------------------------------------------------------

int medusa_image_handler_clear(asset_image* asset);

void medusa_image_handler_load_default_palette(asset_image* asset, void* buffer);

//---------------------------------------------------------------------------------------------------
// Manipulation
//---------------------------------------------------------------------------------------------------

int medusa_image_handler_greyscale(asset_image* src, asset_image* dest);

int medusa_image_handler_stack(asset_image* src, asset_image* dest);

//---------------------------------------------------------------------------------------------------
// Dither
//---------------------------------------------------------------------------------------------------

void medusa_image_handler_dither_stupid(asset_image* src, asset_image* dest);
void medusa_image_handler_dither_gridded(asset_image* src, asset_image* dest);
void medusa_image_handler_dither_floyd_steinberg(asset_image* src, asset_image* dest);
void medusa_image_handler_dither_atkinson(asset_image* src, asset_image* dest);
void medusa_image_handler_dither_jarvis_judice_ninke(asset_image* asset, asset_image* dest);

#endif
