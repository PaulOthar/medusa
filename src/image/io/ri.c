#include "medusa_image/io.h"
#include "generic_implementation.h"

#pragma pack(push, 1)
typedef struct /*header_ri*/{
	uint16_t magic_number;
	uint16_t version;
	uint32_t file_size;

	uint8_t header_offset;
	uint8_t bits_per_pixel;
	uint16_t palette_entries;
	uint16_t width, height;
}header_ri;
#pragma pack(pop)

#define MAGIC_NUMBER 0x4952
#define VERSION_NUMBER 1

//---------------------------------------------------------------------------------------------------

static void _initialize_header(header_ri* header){
	header->magic_number = MAGIC_NUMBER;
	header->version = VERSION_NUMBER;
	header->file_size = sizeof(header_ri);

	header->header_offset = sizeof(header_ri);//Constant

	header->width = 0;
	header->height = 0;
	header->palette_entries = 0;
	header->bits_per_pixel = 32;
}

static void _asset_to_header(asset_image* asset, header_ri* header){
	_initialize_header(header);

	metadata_image* meta = &asset->meta;

	int data_size = (meta->width * meta->height * meta->bits_per_pixel) / 8;
	int palette_size = meta->palette_entries * 4;

	header->file_size = sizeof(header_ri) + data_size + palette_size;

	header->width = meta->width;
	header->height = meta->height;
	header->palette_entries = meta->palette_entries;
	header->bits_per_pixel = meta->bits_per_pixel;

	header->header_offset = sizeof(header_ri);
}

static void _header_to_asset(asset_image* asset, header_ri* header){
	metadata_image* meta = &asset->meta;

	meta->width = header->width;
	meta->height = header->height;
	meta->palette_entries = header->palette_entries;
	meta->bits_per_pixel = header->bits_per_pixel;
}

static int _validate_and_load_header(asset_image* asset, header_ri* header){
	if(header->magic_number != MAGIC_NUMBER){ return -1; }
	_header_to_asset(asset, header);

	return 0;
}

//---------------------------------------------------------------------------------------------------

static int _read_ri(asset_image* asset, void* file, medusa_util_rw reader){
	header_ri header = {0}; reader(&header, sizeof(header_ri), 1, file, 0);
	_validate_and_load_header(asset, &header);

	medusa_image_update_effective_sizes(asset);
	int code = medusa_image_validate_buffers(asset);

	if(code < 0){ return code; }

	int file_offset = sizeof(header_ri);

	file_offset += _medusa_file_read_palette(asset, file, reader, file_offset, code);

	if((code & IMAGE_COMMON_CODE_MISSING_DATA) == 0){//If no data error was thrown
		if(header.bits_per_pixel == 32){ file_offset = header.header_offset; }
		reader(asset->data, 1, asset->effective_data_size, file, file_offset);
	}

	return code;
}

static int _write_ri(asset_image* asset, void* file, medusa_util_rw writter){
	medusa_image_update_effective_sizes(asset);
	int code = medusa_image_validate_buffers(asset);

	int file_offset = sizeof(header_ri);

	file_offset += _medusa_file_write_palette(asset, file, writter, file_offset, code);

	if((code & IMAGE_COMMON_CODE_MISSING_DATA) == 0){//If no data error was thrown
		writter(asset->data, 1, asset->effective_data_size, file, file_offset);
	}

	header_ri header = {0}; _asset_to_header(asset, &header);
	writter(&header, sizeof(header_ri), 1, file, 0);

	return code;
}

//---------------------------------------------------------------------------------------------------

int medusa_image_io_read_ri(asset_image* asset, void* data){
	return _read_ri(asset, data, (medusa_util_rw)medusa_util_seek_and_read_memory);
}

int medusa_image_io_write_ri(asset_image* asset, void* data){
	return _write_ri(asset, data, (medusa_util_rw)medusa_util_seek_and_write_memory);
}

int medusa_image_io_read_file_ri(asset_image* asset, FILE* file){
	return _read_ri(asset, file, (medusa_util_rw)medusa_util_seek_and_read_file);
}

int medusa_image_io_write_file_ri(asset_image* asset, FILE* file){
	return _write_ri(asset, file, (medusa_util_rw)medusa_util_seek_and_write_file);
}
