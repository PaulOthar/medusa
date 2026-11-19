#include "medusa_image/handler.h"

int medusa_image_handler_clear(asset_image* asset){
	int code = medusa_image_validate_buffers(asset);
	if(code & IMAGE_COMMON_CODE_INVALID_DATA){ return code; }//can't operate on invalid buffers

	uint8_t* bytes = asset->data;
	int size = asset->effective_data_size;
	for(int i = 0; i < size; i++){ bytes[i] = 0; }

	return code;
}

static uint32_t default_colors[] = {
	0xff00ff, 0x010101, 0x818181, 0xffffff,
	0xff0000, 0xaa0000, 0x550000, 0x2a0000,
	0x00ff00, 0x00aa00, 0x005500, 0x002a00,
	0x00ffff, 0x00aaaa, 0x005555, 0x002a2a
};

int medusa_image_handler_write_default_palette(int entries, int size, void* buffer){
	uint32_t* buff = buffer;
	uint32_t color = 0;

	size /= 4;
	int e = 0;//entries written

	if(entries <= 2){
		size = size > 2 ? 2 : size;
		for(; e < size; e++, color += 0xffffff){ buff[e] = color; }
	} else if(entries <= 16){
		size = size > 16 ? 16 : size;
		for(; e < size; e++){ buff[e] = default_colors[e]; }
	} else if(entries <= 256){
		size = size > 256 ? 256 : size;
		for(; e < size; e++, color += 0x010101){ buff[e] = color; }
	}

	return e;
}

int medusa_image_handler_load_default_palette(asset_image* asset){
	int code = medusa_image_validate_buffers(asset);
	if(code & IMAGE_COMMON_CODE_INVALID_PALT){ return code; }//can't operate on invalid buffers

	int entries = asset->meta.palette_entries;
	int size = asset->palette_size;//physical entries
	void* buffer = asset->palette;

	int written = medusa_image_handler_write_default_palette(entries, size, buffer);

	asset->effective_palette_size = written * 4;
	asset->meta.palette_entries = written;

	return written;
}

