#include "image/medusa_image_io.h"

enum image_format_mode{
	READ = 0,
	WRITE = 1
};

static const medusa_image_io io_table[3][2] = {
	[IMAGE_FORMAT_BMP_MEMORY] = {medusa_image_io_read_bmp, medusa_image_io_write_bmp},
	[IMAGE_FORMAT_BMP_FILE] = {(medusa_image_io)medusa_image_io_read_bmp_file, (medusa_image_io)medusa_image_io_write_bmp_file}
};

int medusa_image_io_asset_read(void* source, int format, void* block){
	medusa_image_io func = io_table[format][READ];

	asset_image dummy = {0};
	func(&dummy, source);

	int size = sizeof(asset_image) + dummy.data_size + dummy.palette_size;
	if(!block){ return size; }

	uint8_t* buffer = (uint8_t*)block;

	asset_image* result = (asset_image*)buffer; result[0] = dummy; buffer += sizeof(asset_image);
	result->data = buffer; buffer += dummy.data_size;
	result->palette = dummy.palette_size ? buffer : 0;

	func(result, source);

	return size;
}
