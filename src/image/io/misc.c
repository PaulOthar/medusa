#include "../../../include/medusa_image/io.h"

enum image_format_mode{
	READ = 0,
	WRITE = 1
};

int medusa_image_io_asset_read(void* source, medusa_image_io reader, void* block){
	asset_image dummy = {0};
	reader(&dummy, source);

	int size = sizeof(asset_image) + dummy.data_size + dummy.palette_size;
	if(!block){ return size; }

	uint8_t* buffer = (uint8_t*)block;

	asset_image* result = (asset_image*)buffer; result[0] = dummy; buffer += sizeof(asset_image);
	result->data = buffer; buffer += dummy.data_size;
	result->palette = dummy.palette_size ? buffer : 0;

	reader(result, source);

	return size;
}
