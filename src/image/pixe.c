#include "image/pixel.h"

uint32_t image_pixel_to_greyscale(uint32_t color){
	uint8_t* argb = (uint8_t*)&color;

	uint32_t acc = 500;
	acc += 114 * argb[0];//blue
	acc += 587 * argb[1];//green
	acc += 299 * argb[2];//red
	acc /= 1000;

	return acc * 0x010101;
}
