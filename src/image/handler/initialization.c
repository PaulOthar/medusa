#include "../../../include/medusa_image/handler.h"

int medusa_image_handler_clear(asset_image* asset){
	int code = medusa_image_validate_buffers(asset);
	if(code){ return code; }

	uint8_t* bytes = asset->data;
	int size = asset->data_size;
	for(int i = 0; i < size; i++){ bytes[i] = 0; }

	return code;
}

static uint32_t default_colors[] = {
	0xff00ff, 0x010101, 0x818181, 0xffffff,
	0xff0000, 0xaa0000, 0x550000, 0x2a0000,
	0x00ff00, 0x00aa00, 0x005500, 0x002a00,
	0x00ffff, 0x00aaaa, 0x005555, 0x002a2a
};

int medusa_image_handler_load_default_palette(asset_image* asset){
	uint32_t* buff = asset->palette;
	if(!buff){ return 0; }

	int size = (asset->palette_size) / 4;
	int col = 0;
	int e = 0;

	switch(asset->meta.bits_per_pixel){
		case 1:
			size = size > 2 ? 2 : size;
			for(; e < size; e++, col += 0xffffff){ buff[e] = col; }
			break;
		case 4:
			size = size > 16 ? 16 : size;
			for(; e < size; e++){ buff[e] = default_colors[e]; }
			break;
		case 8://Pretty much grey scale
			size = size > 256 ? 256 : size;
			for(; e < size; e++, col += 0x010101){ buff[e] = col; }
			break;
		default:
			return e;
	}

	asset->effective_palette_size = size * 4;
	asset->meta.palette_entries = size;

	return e;
}

