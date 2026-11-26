#include "generic_implementation.h"

int _medusa_file_read_palette(asset_image* asset, void* file, medusa_util_rw reader, int file_offset, int code){
	int read = 0;

	if((code & IMAGE_COMMON_CODE_MISSING_PALT) == 0){//If no palette error was thrown
		reader(asset->palette, 1, asset->effective_palette_size, file, file_offset);
		read = asset->effective_palette_size;
	}

	return read;
}

int _medusa_file_write_palette(asset_image* asset, void* file, medusa_util_rw writter, int file_offset, int code){
	int written = 0;

	if((code & IMAGE_COMMON_CODE_MISSING_PALT) == 0){//If no palette error was thrown
		writter(asset->palette, 1, asset->effective_palette_size, file, file_offset);
		written = asset->effective_palette_size;
	} else if(asset->meta.palette_entries){
		int entries = asset->meta.palette_entries;
		int size = asset->effective_palette_size;
		uint8_t buffer[asset->effective_palette_size];
		medusa_image_handler_write_default_palette(entries, size, buffer);

		writter(buffer, 1, size, file, file_offset);

		written = size;
	}

	return written;
}
