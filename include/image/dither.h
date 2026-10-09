#ifndef IMAGE_DITHER
#define IMAGE_DITHER

#include "image/common.h"

void image_dither_stupid(asset_image* asset);

void image_dither_gridded(asset_image* asset);

void image_dither_floyd_steinberg(asset_image* asset);

void image_dither_atkinson(asset_image* asset);

void image_dither_jarvis_judice_ninke(asset_image* asset);

#endif
