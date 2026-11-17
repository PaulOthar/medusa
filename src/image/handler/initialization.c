#include "image/medusa_image_handler.h"

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

void medusa_image_handler_load_default_palette(asset_image* asset, void* buffer){
	uint32_t* dest_word = buffer;
	if(!buffer){ return; }

	switch(asset->meta.bits_per_pixel){
	case 1:
		dest_word[0] = 0;
		dest_word[1] = -1;
		break;
	case 4:
		for(int i = 0; i < 16; i++){ dest_word[i] = default_colors[i]; }
		break;
	case 8:{//Pretty much grey scale
		for(int i = 0; i < 256; i++){ dest_word[i] = i * 0x010101; }
		break;
	}
	default:
		break;
	}
}
