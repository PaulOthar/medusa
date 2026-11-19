#include "../../../include/medusa_image/handler.h"
#include "../../../include/medusa_image/handler.h"
#include "../../../include/medusa_image/io.h"
#include "medusa_util.h"

#pragma pack(push, 1)
typedef struct /*header_bmp*/{
	uint16_t magic_number;
	uint32_t file_size;
	uint32_t reserved;
	uint32_t header_offset;//54 = 14 + 40

	uint32_t header_extention_size;//40
	uint32_t width;
	uint32_t height;
	uint16_t color_planes;//1
	uint16_t bits_per_pixel;//1, 4, 8, 16, 24, 32
	uint32_t compression;//0
	uint32_t data_size;//(in bytes) Just the pixel data, not the palette.
	uint32_t biXPelsPerMeter;//(???) 2834
	uint32_t biYPelsPerMeter;//(???) 2834
	uint32_t palette_count;
	uint32_t palette_important_count;
}header_bmp;
#pragma pack(pop)

//---------------------------------------------------------------------------------------------------

static void _initialize_header(header_bmp* header){
	header->magic_number = 0x4D42;
	header->file_size = sizeof(header_bmp);
	header->reserved = 0;//Constant
	header->header_offset = sizeof(header_bmp);//Constant

	header->header_extention_size = 40;//Constant
	header->width = 0;
	header->height = 0;
	header->color_planes = 1;//Constant
	header->bits_per_pixel = 32;
	header->compression = 0;//Constant
	header->data_size = 0;
	header->biXPelsPerMeter = 2834;//Constant
	header->biYPelsPerMeter = 2834;//Constant
	header->palette_count = 0;
	header->palette_important_count = 0;
}

static void _asset_to_header(asset_image* asset, header_bmp* header){
	_initialize_header(header);

	metadata_image* meta = &asset->meta;

	header->width = meta->width;
	header->height = meta->height;
	header->bits_per_pixel = meta->bits_per_pixel;
	header->data_size = (meta->width * meta->height * meta->bits_per_pixel) / 8;
	header->palette_count = meta->palette_entries;
	header->palette_important_count = meta->palette_entries;

	header->file_size = sizeof(header_bmp) + header->data_size + (header->palette_count * 4);
	header->header_offset = sizeof(header_bmp) + (meta->palette_entries * 4);
}

static void _header_to_asset(asset_image* asset, header_bmp* header){
	metadata_image* meta = &asset->meta;

	meta->width = header->width;
	meta->height = header->height;
	meta->bits_per_pixel = header->bits_per_pixel;
	meta->palette_entries = header->palette_count;
}

static int _validate_and_load_header(asset_image* asset, header_bmp* header){
	if(header->magic_number != 0x4D42){ return -1; }
	_header_to_asset(asset, header);

	return 0;
}

//---------------------------------------------------------------------------------------------------

static void _fix_bitmap_orientations(asset_image* asset){
	//Fixes line orientation (flipping it upside down)
	medusa_image_handler_flip_vertical(asset, asset);
	if(asset->meta.bits_per_pixel >= 16){ return; }
	//Fixes bits orientations (0xA7 becomes 0x7A in 4bpp)
	medusa_util_switch_bits_positions(asset->data, asset->data_size, asset->meta.bits_per_pixel);
}

//---------------------------------------------------------------------------------------------------

static int _read_lines(asset_image* asset, void* file, int offset, medusa_util_rw reader){
	int padding = (asset->meta.width * asset->meta.bits_per_pixel) % 32;//Each line must be 4 bytes aligned
	padding = (32 - padding) % 32;

	if(!padding){
		int data_size = asset->effective_data_size;
		reader(asset->data, 1, data_size, file, offset);
		_fix_bitmap_orientations(asset);
		return 0;
	}

	/* If it reached here, then we have a file with an unpadded size of pixels
	The solution is to:
		1. Extract all bits in a line, to a padded buffer (multiple of 32 bits).
		2. Pack all the unpadded bits to the data buffer.
	*/

	int lines = asset->meta.height;
	int line_size = asset->meta.width * asset->meta.bits_per_pixel;
	int line_bytes = (padding + line_size) / 8;
	uint8_t line_buffer[line_bytes];

	offset += line_bytes * (lines - 1);
	int data_addr = 0;
	for(int i = 0; i < lines; i++){
		reader(line_buffer, 1, line_bytes, file, offset);
		medusa_util_switch_bits_positions(line_buffer, line_bytes, asset->meta.bits_per_pixel);
		medusa_util_copy_bits_new(line_buffer, 0, asset->data, data_addr, line_size);
		data_addr += line_size;
		offset -= line_bytes;
	}

	return 0;
}

static int _write_lines(asset_image* asset, void* file, int offset, medusa_util_rw writter){
	int padding = (asset->meta.width * asset->meta.bits_per_pixel) % 32;//Each line must be 4 bytes aligned
	padding = (32 - padding) % 32;

	if(!padding){
		int data_size = asset->effective_data_size;
		_fix_bitmap_orientations(asset);
		writter(asset->data, 1, data_size, file, offset);
		_fix_bitmap_orientations(asset);
		//We called it twice, so the in memory image would not end up screwed
		return 0;
	}

	/* If it reached here, then we have a file with an unpadded size of pixels
	The solution is to:
		1. Extract all bits in a line, to a padded buffer (multiple of 32 bits).
		2. Write said buffer to the file.
	*/

	int lines = asset->meta.height;
	int line_size = asset->meta.width * asset->meta.bits_per_pixel;
	int line_bytes = (padding + line_size) / 8;
	uint8_t line_buffer[line_bytes];

	int data_addr = 0; offset += line_bytes * (lines - 1);
	for(int i = 0; i < lines; i++){
		medusa_util_copy_bits_new(asset->data, data_addr, line_buffer, 0, line_size);
		medusa_util_switch_bits_positions(line_buffer, line_bytes, asset->meta.bits_per_pixel);
		writter(line_buffer, 1, line_bytes, file, offset);
		data_addr += line_size;
		offset -= line_bytes;
	}

	return 0;
}

//---------------------------------------------------------------------------------------------------

static int _read_bmp(asset_image* asset, void* file, medusa_util_rw reader){
	header_bmp header = {0}; reader(&header, sizeof(header_bmp), 1, file, 0);
	_validate_and_load_header(asset, &header);

	medusa_image_update_effective_sizes(asset);
	int code = medusa_image_validate_buffers(asset);

	if(code < 0){ return code; }

	int file_offset = sizeof(header_bmp);

	if((code & IMAGE_COMMON_CODE_MISSING_PALT) == 0){//If no palette error was thrown
		reader(asset->palette, 1, asset->palette_size, file, file_offset);
	}

	file_offset += asset->effective_palette_size;

	if((code & IMAGE_COMMON_CODE_MISSING_DATA) == 0){//If no data error was thrown
		if(header.bits_per_pixel == 32){ file_offset = header.header_offset; }
		code |= _read_lines(asset, file, file_offset, reader);
	}

	return code;
}

static int _write_bmp(asset_image* asset, void* file, medusa_util_rw writter){
	medusa_image_update_effective_sizes(asset);
	int code = medusa_image_validate_buffers(asset);

	int file_offset = sizeof(header_bmp);

	if((code & IMAGE_COMMON_CODE_MISSING_PALT) == 0){//If no palette error was thrown
		writter(asset->palette, 1, asset->effective_palette_size, file, file_offset);
	} else if(asset->meta.palette_entries){
		void* prev_palette = asset->palette;
		uint8_t palette[asset->effective_palette_size];

		asset->palette = palette;
		medusa_image_handler_load_default_palette(asset);
		asset->palette = prev_palette;

		writter(palette, 1, asset->effective_palette_size, file, file_offset);
	}

	file_offset += asset->effective_palette_size;

	if((code & IMAGE_COMMON_CODE_MISSING_DATA) == 0){//If no data error was thrown
		code |= _write_lines(asset, file, file_offset, writter);
	}

	header_bmp header = {0}; _asset_to_header(asset, &header);
	writter(&header, sizeof(header_bmp), 1, file, 0);

	return code;
}

//---------------------------------------------------------------------------------------------------

int medusa_image_io_read_bmp(asset_image* asset, void* data){
	return _read_bmp(asset, data, (medusa_util_rw)medusa_util_seek_and_read_memory);
}

int medusa_image_io_write_bmp(asset_image* asset, void* data){
	return _write_bmp(asset, data, (medusa_util_rw)medusa_util_seek_and_write_memory);
}

int medusa_image_io_read_bmp_file(asset_image* asset, FILE* file){
	return _read_bmp(asset, file, (medusa_util_rw)medusa_util_seek_and_read_file);
}

int medusa_image_io_write_bmp_file(asset_image* asset, FILE* file){
	return _write_bmp(asset, file, (medusa_util_rw)medusa_util_seek_and_write_file);
}
