#include <stdlib.h>

#include "medusa_util.h"
#include "image/medusa_image_dither.h"
#include "image/medusa_image_pixel.h"
//I really should not be depending on malloc for big buffers....... (plys, future me, fix it)

void _greyscale_8bpp(asset_image* asset, int16_t* gsb, int size){
	for(int i = 0; i < size; i++){
		uint32_t pixel = medusa_image_common_get_pixel(asset, i);
		pixel = medusa_image_pixel_to_greyscale(pixel);
		gsb[i] = pixel & 0xff;
	}
}

void _collapse_image_dither(asset_image* asset, int16_t* gs){
	uint8_t* data = asset->data;
	for(int i = 0; i < asset->data_size; i++){ data[i] = 0; }

	for(int y = 0, byte = 0, pushed = 0; y < asset->meta.height; y++){
		int ysum = asset->meta.width * y;
		for(int x = 0; x < asset->meta.width; byte++){
			for(int b = 0; b < 8 && x < asset->meta.width; b++, x++, pushed++){
				uint8_t bit = gs[x + ysum] && 1;
				medusa_util_push_bit(asset->data, bit, pushed);
			}
		}
	}

	asset->meta.palette_entries = 2;
	asset->meta.bits_per_pixel = 1;
}

//---------------------------------------------------------------------------------------------------

void medusa_image_dither_stupid(asset_image* asset){
	int size = medusa_image_common_calculate_image_size(asset);
	int16_t* gs = (int16_t*)malloc(size * sizeof(uint16_t));
	_greyscale_8bpp(asset, gs, size);

	for(int i = 0; i < size; i++){ gs[i] = (gs[i] >> 7) * 0xff; }

	_collapse_image_dither(asset, gs);
	free(gs);
}

void medusa_image_dither_gridded(asset_image* asset){
	int width = asset->meta.width, height = asset->meta.height, size = width * height;
	int16_t* gs = (int16_t*)malloc(size * sizeof(uint16_t));
	_greyscale_8bpp(asset, gs, size);

	int grade = 256 / 7;

	for(int y = 0, yptr = 0; y < height; y++, yptr += width){
		for(int x = 0; x < width; x++){
			int mean = gs[x + yptr];

			if(mean >= grade * 6){ gs[x + yptr] = 0xFF; continue; }
			if(mean >= grade * 5){
				if(((y & 3) == 0) && ((x & 3) == 0)){ gs[x + yptr] = 0x00; continue; }
				if(((y & 3) == 2) && ((x & 3) == 2)){ gs[x + yptr] = 0x00; continue; }
				gs[x + yptr] = 0xFF; continue;
			}
			if(mean >= grade * 4){ gs[x + yptr] = ((x & 1) || (y & 1)) ? 0x00 : 0xFF; continue; }
			if(mean >= grade * 3){ gs[x + yptr] = ((x & 1) ^ (y & 1)) ? 0x00 : 0xFF; continue; }
			if(mean >= grade * 2){ gs[x + yptr] = ((x & 1) || (y & 1)) ? 0xFF : 0x00; continue; }
			if(mean >= grade * 1){
				if(((y & 3) == 0) && ((x & 3) == 0)){ gs[x + yptr] = 0xFF; continue; }
				if(((y & 3) == 2) && ((x & 3) == 2)){ gs[x + yptr] = 0xFF; continue; }
				gs[x + yptr] = 0; continue;
			}
			gs[x + yptr] = 0; continue;
		}
	}

	_collapse_image_dither(asset, gs);
	free(gs);
}

void medusa_image_dither_floyd_steinberg(asset_image* asset){
	int width = asset->meta.width, height = asset->meta.height, size = width * height;
	int16_t* gs = (int16_t*)malloc(size * sizeof(uint16_t));
	_greyscale_8bpp(asset, gs, size);

	for(int y = 0, yptr = 0; y < height; y++, yptr += width){
		for(int x = 0; x < width; x++){
			int ptr = x + yptr;
			int oldpx = gs[ptr];
			int newpx = oldpx > 127 ? 255 : 0;

			gs[ptr] = newpx;
			int error = oldpx - newpx;
			error /= 8;

			if((x + 1) < width){ gs[ptr + 1] += error; }
			if((x + 2) < width){ gs[ptr + 2] += error; }
			if((y + 1) >= height){ continue; }
			if((x + 1) < width){ gs[ptr + 1 + width] += error; }
			gs[ptr + width] += error;
			if(x > 0){ gs[ptr - 1 + width] += error; }
			if((y + 2) >= height){ continue; }
			gs[ptr + (width * 2)] += error;
		}
	}

	_collapse_image_dither(asset, gs);
	free(gs);
}

void medusa_image_dither_atkinson(asset_image* asset){
	int width = asset->meta.width, height = asset->meta.height, size = width * height;
	int16_t* gs = (int16_t*)malloc(size * sizeof(uint16_t));
	_greyscale_8bpp(asset, gs, size);

	for(int y = 0, yptr = 0; y < height; y++, yptr += width){
		for(int x = 0; x < width; x++){
			int ptr = x + yptr;
			int oldpx = gs[ptr];
			int newpx = oldpx > 127 ? 255 : 0;

			gs[ptr] = newpx;
			int error = oldpx - newpx;
			error /= 8;

			if((x + 1) < width){ gs[ptr + 1] += error; }
			if((x + 2) < width){ gs[ptr + 2] += error; }
			if((y + 1) >= height){ continue; }
			if((x + 1) < width){ gs[ptr + 1 + width] += error; }
			gs[ptr + width] += error;
			if(x > 0){ gs[ptr - 1 + width] += error; }
			if((y + 2) >= height){ continue; }
			gs[ptr + (width * 2)] += error;
		}
	}

	_collapse_image_dither(asset, gs);
	free(gs);
}

void medusa_image_dither_jarvis_judice_ninke(asset_image* asset){
	int width = asset->meta.width, height = asset->meta.height, size = width * height;
	int16_t* gs = (int16_t*)malloc(size * sizeof(uint16_t));
	_greyscale_8bpp(asset, gs, size);

	for(int y = 0, yptr = 0; y < height; y++, yptr += width){
		for(int x = 0; x < width; x++){
			int ptr = x + yptr;
			int oldpx = gs[ptr];
			int newpx = oldpx > 127 ? 255 : 0;

			gs[ptr] = newpx;
			int error = oldpx - newpx;

			int err7 = error * 7.0 / 48.0;
			int err5 = error * 5.0 / 48.0;
			int err3 = error * 3.0 / 48.0;
			int err1 = error * 1.0 / 48.0;

			if((x + 1) < width){ gs[ptr + 1] += err7; }
			if((x + 2) < width){ gs[ptr + 2] += err5; }

			if((y + 1) >= height){ continue; }
			if(x > 1){ gs[ptr + width - 2] += err3; }
			if(x > 0){ gs[ptr + width - 1] += err5; }
			gs[ptr + width] += err7;
			if((x + 1) < width){ gs[ptr + width + 1] += err5; }
			if((x + 2) < width){ gs[ptr + width + 2] += err3; }

			if((y + 2) >= height){ continue; }
			int w2 = width * 2;
			if(x > 1){ gs[ptr + w2 - 2] += err1; }
			if(x > 0){ gs[ptr + w2 - 1] += err3; }
			gs[ptr + w2] += err5;
			if((x + 1) < width){ gs[ptr + w2 + 1] += err3; }
			if((x + 2) < width){ gs[ptr + w2 + 2] += err1; }
		}
	}

	_collapse_image_dither(asset, gs);
	free(gs);
}

