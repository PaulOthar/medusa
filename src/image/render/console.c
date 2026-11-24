#include <stdio.h>
#include "medusa_image/render.h"
#include "medusa_image/util.h"

#define TOP_PIXEL_CHAR "▀"
#define BOT_PIXEL_CHAR "▄"
#define FULL_PIXEL_CHAR "█"
#define EMPTY_PIXEL_CHAR " "

#define COLOR_TOP(RGB) printf("\x1b[38;2;%d;%d;%dm", (RGB >> 16) & 0xff, (RGB >> 8) & 0xff, RGB & 0xff);
#define COLOR_BOT(RGB) printf("\x1b[48;2;%d;%d;%dm", (RGB >> 16) & 0xff, (RGB >> 8) & 0xff, RGB & 0xff);
#define COLOR_RESET printf("\x1b[0m");

static int _render_dual_line(asset_image* asset, int index, int width){
	int index_2 = index + width;

	medusa_image_get_function get = medusa_image_function_get_pixel(asset->meta.bits_per_pixel);

	uint32_t prev_top = 0; COLOR_TOP(prev_top);
	uint32_t prev_bottom = 0; COLOR_BOT(prev_bottom);
	for(int i = 0; i < width; i++, index++, index_2++){
		uint32_t top = get(asset, index);
		uint32_t bottom = get(asset, index_2);

		if(top != prev_top){ prev_top = top; COLOR_TOP(prev_top); }
		if(bottom != prev_bottom){ prev_bottom = bottom; COLOR_BOT(prev_bottom); }

		printf(TOP_PIXEL_CHAR);
	}

	COLOR_RESET;
	printf("\n");

	return index_2;
}

static int _render_line(asset_image* asset, int index, int width){
	medusa_image_get_function get = medusa_image_function_get_pixel(asset->meta.bits_per_pixel);

	uint32_t prev_top = 0; COLOR_TOP(prev_top);
	for(int i = 0; i < width; i++, index++){
		uint32_t top = get(asset, index);

		if(top != prev_top){ prev_top = top; COLOR_TOP(prev_top); }

		printf(TOP_PIXEL_CHAR);
	}

	COLOR_RESET;
	printf("\n");

	return index;
}

void medusa_image_render_console_ansi(asset_image* asset){
	int lines = asset->meta.height >> 1;
	int width = asset->meta.width;
	int index = 0;

	void* prev_palette = asset->palette;
	int ideal_palette_size = medusa_image_util_format_placeholder_palette(asset, 0);
	uint8_t placeholder_palette[ideal_palette_size];
	if(asset->palette_size < ideal_palette_size){
		medusa_image_util_format_placeholder_palette(asset, placeholder_palette);
	}

	for(int i = 0; i < lines; i++){
		index = _render_dual_line(asset, index, width);
	}

	if(asset->meta.height & 1){
		_render_line(asset, index, width);
	}

	asset->palette = prev_palette;
}
