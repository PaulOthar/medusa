#include <string.h>

#include "medusa_util.h"
#include "image/medusa_image_io.h"

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

	return medusa_image_common_validate_buffers(asset);
}

static void _fix_line_position(void* data, int width, int height, int bits_per_pixel){
	uint8_t* ptr = data;

	int line_bytes = (width * bits_per_pixel) / 8;

	int index_top = 0;
	int index_bottom = (height - 1) * line_bytes;

	for(;index_top < index_bottom; index_top += line_bytes, index_bottom -= line_bytes){
		for(int i = 0; i < line_bytes; i++){
			int temp = ptr[i + index_top];
			ptr[i + index_top] = ptr[i + index_bottom];
			ptr[i + index_bottom] = temp;
		}
	}
}

static void _fix_byte_orientation(void* data, int index, int bits_per_pixel){
	uint8_t* ptr = data;
	int mask = (1 << bits_per_pixel) - 1;

	int fix_buffer = 0;
	int byte = ptr[index];

	for(int l = 0; l < 8; l += bits_per_pixel){
		fix_buffer <<= bits_per_pixel;
		fix_buffer |= byte & mask;
		byte >>= bits_per_pixel;
	}

	ptr[index] = fix_buffer;
}

static void _fix_bytes_orientation(void* data, int count, int bits_per_pixel){
	if(bits_per_pixel >= 16){ return; }
	for(int i = 0; i < count; i++){ _fix_byte_orientation(data, i, bits_per_pixel); }
}

static void _fix_bit_position(void* data, int width, int height, int bits_per_pixel){
	uint8_t* ptr = data;

	int mask = (1 << bits_per_pixel) - 1;
	int total_size = (width * height * bits_per_pixel) / 8;

	for(int i = 0; i < total_size; i++){
		int fix_buffer = 0;
		int byte = ptr[i];

		for(int l = 0; l < 8; l += bits_per_pixel){
			fix_buffer <<= bits_per_pixel;
			fix_buffer |= byte & mask;
			byte >>= bits_per_pixel;
		}

		ptr[i] = fix_buffer;
	}
}

static void _fix_bitmap_asset(asset_image* asset){
	_fix_line_position(asset->data, asset->meta.width, asset->meta.height, asset->meta.bits_per_pixel);
	if(asset->meta.bits_per_pixel >= 16){ return; }
	_fix_bit_position(asset->data, asset->meta.width, asset->meta.height, asset->meta.bits_per_pixel);
}

static void _fix_bitmap_anonymous(asset_image* asset, void* anon){
	_fix_line_position(anon, asset->meta.width, asset->meta.height, asset->meta.bits_per_pixel);
	if(asset->meta.bits_per_pixel >= 16){ return; }
	_fix_bit_position(anon, asset->meta.width, asset->meta.height, asset->meta.bits_per_pixel);
}

//---------------------------------------------------------------------------------------------------

int _format_image_line(asset_image* asset, int line_index, void* buffer){
	int line_size = asset->meta.width * asset->meta.bits_per_pixel;

	if(!buffer){
		int padding = (asset->meta.width * asset->meta.bits_per_pixel) % 32;//Each line must be 4 bytes aligned
		padding = (32 - padding) % 32;
		return (padding + line_size) / 8;
	}

	int offset = line_size * line_index;

	medusa_util_copy_bits(asset->data, buffer, offset, line_size);

	return 0;
}

//---------------------------------------------------------------------------------------------------

static void _seek_and_read(void* ptr, int size, int length, FILE* file, int offset){
	fseek(file, offset, SEEK_SET);
	fread(ptr, size, length, file);
}

static void _seek_and_write(void* ptr, int size, int length, FILE* file, int offset){
	fseek(file, offset, SEEK_SET);
	fwrite(ptr, size, length, file);
}

static int _read_file_lines(asset_image* asset, FILE* file, int offset){
	int padding = (asset->meta.width * asset->meta.bits_per_pixel) % 32;//Each line must be 4 bytes aligned
	padding = (32 - padding) % 32;

	if(!padding){
		int data_size = medusa_image_common_calculate_data_size(asset);
		_seek_and_read(asset->data, 1, data_size, file, offset);
		_fix_bitmap_asset(asset);
		return 0;
	}

	return IMAGE_COMMON_CODE_INVALID_SIZE;//TODO implement any type reading

//	int lines = asset->meta.height;
//	int line_size = asset->meta.width * asset->meta.bits_per_pixel;
}

static int _write_file_lines(asset_image* asset, FILE* file, int offset){
	int padding = (asset->meta.width * asset->meta.bits_per_pixel) % 32;//Each line must be 4 bytes aligned
	padding = (32 - padding) % 32;

	if(!padding){
		int data_size = medusa_image_common_calculate_data_size(asset);
		_fix_bitmap_asset(asset);//Expensive to convert twice, but still... its needed...
		_seek_and_write(asset->data, 1, data_size, file, offset);
		_fix_bitmap_asset(asset);
		return 0;
	}

//	return IMAGE_COMMON_CODE_INVALID_SIZE;//TODO implement any type writting

	int lines = asset->meta.height;
	int line_bytes = _format_image_line(asset, 0, 0);
	uint8_t line_buffer[line_bytes];

	offset += line_bytes * (lines - 1);
	for(int i = 0; i < lines; i++){
		_format_image_line(asset, i, line_buffer);
		_fix_bytes_orientation(line_buffer, line_bytes, asset->meta.bits_per_pixel);
		_seek_and_write(line_buffer, 1, line_bytes, file, offset);
		offset -= line_bytes;
	}

	return 0;

//	int lines = asset->meta.height;
//	int line_size = asset->meta.width * asset->meta.bits_per_pixel;
//
//	int line_bytes = (padding + line_size) / 8;
//	uint8_t line_buffer[line_bytes];
//
//	offset += line_bytes * (lines - 1);
//
//	uint8_t* data = asset->data;
//	int datoff = 0;
//	for(int i = 0; i < lines; i++){
//		_cpy_b(data, line_buffer, datoff, line_size); datoff += line_size;
//		if(asset->meta.bits_per_pixel < 16){
//			for(int l = 0; l < line_bytes; l++){ _fix_byte_orientation(line_buffer, l, asset->meta.bits_per_pixel); }
//		}
//
//		_seek_and_write(line_buffer, 1, line_bytes, file, offset);
//		offset -= line_bytes;
//	}
//
//	return 0;
}

//---------------------------------------------------------------------------------------------------

//TODO ON READ/WRITE, THE SYSTEM MUST ACCOUNT FOR LESS THAN 4 BYTES LINE WIDTH (Pending)

int medusa_image_io_read_bmp(asset_image* asset, void* data){
	int code = _validate_and_load_header(asset, data);
	if(code < 0){ return code; }

	uint8_t* steppable_data = data;
	steppable_data += sizeof(header_bmp);

	if((code & IMAGE_COMMON_CODE_MISSING_PALT) == 0){//If no palette error was thrown
		memcpy(asset->palette, steppable_data, asset->palette_size);
	}

	steppable_data += medusa_image_common_calculate_palette_size(asset);

	if((code & IMAGE_COMMON_CODE_MISSING_DATA) == 0){//If no data error was thrown
		memcpy(asset->data, steppable_data, asset->data_size);
		_fix_bitmap_asset(asset);
	}

	return code;
}

int medusa_image_io_write_bmp(asset_image* asset, void* data){
	_asset_to_header(asset, data);

	int code = medusa_image_common_validate_buffers(asset);

	uint8_t* steppable_data = data;
	steppable_data += sizeof(header_bmp);

	if((code & IMAGE_COMMON_CODE_MISSING_PALT) == 0){//If no palette error was thrown
		memcpy(steppable_data, asset->palette, asset->palette_size);
	} else if(asset->meta.palette_entries){
		medusa_image_common_load_default_palette(asset, steppable_data);
	}

	steppable_data += medusa_image_common_calculate_palette_size(asset);

	if((code & IMAGE_COMMON_CODE_MISSING_DATA) == 0){//If no data error was thrown
		_fix_bitmap_anonymous(asset, steppable_data);
	}

	return code;
}

int medusa_image_io_read_bmp_file(asset_image* asset, FILE* file){
	header_bmp header = {0}; _seek_and_read(&header, sizeof(header_bmp), 1, file, 0);

	int code = _validate_and_load_header(asset, &header);
	if(code < 0){ return code; }

	int file_offset = sizeof(header_bmp);

	if((code & IMAGE_COMMON_CODE_MISSING_PALT) == 0){//If no palette error was thrown
		_seek_and_read(asset->palette, 1, asset->palette_size, file, file_offset);
	}

	file_offset += medusa_image_common_calculate_palette_size(asset);

	if((code & IMAGE_COMMON_CODE_MISSING_DATA) == 0){//If no data error was thrown
		if(header.bits_per_pixel == 32){ file_offset = header.header_offset; }
		code |= _read_file_lines(asset, file, file_offset);
	}

	return code;
}

int medusa_image_io_write_bmp_file(asset_image* asset, FILE* file){
	header_bmp header = {0}; _asset_to_header(asset, &header);
	_seek_and_write(&header, sizeof(header_bmp), 1, file, 0);

	int code = medusa_image_common_validate_buffers(asset);

	int file_offset = sizeof(header_bmp);
	int palette_size = medusa_image_common_calculate_palette_size(asset);

	if((code & IMAGE_COMMON_CODE_MISSING_PALT) == 0){//If no palette error was thrown
		_seek_and_write(asset->palette, 1, asset->palette_size, file, file_offset);
	} else if(asset->meta.palette_entries){
		uint8_t palette[palette_size];
		medusa_image_common_load_default_palette(asset, palette);
		_seek_and_write(palette, 1, palette_size, file, file_offset);
	}

	file_offset += palette_size;

	if((code & IMAGE_COMMON_CODE_MISSING_DATA) == 0){//If no data error was thrown
		code |= _write_file_lines(asset, file, file_offset);
	}

	return code;
}
