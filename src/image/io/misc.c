#include "medusa_image/io.h"
#include "medusa_image/common.h"

int medusa_image_io_read_asset(void* source, medusa_image_io reader, void* block){
	if(!block){
		asset_image dummy = {0};
		reader(&dummy, source);
		return sizeof(asset_image) + dummy.effective_data_size + dummy.effective_palette_size;
	}

	asset_image* asset = (asset_image*)block;
	asset[0] = (asset_image){0};

	reader(asset, source);

	uint8_t* u8_block = (uint8_t*)block;
	u8_block += sizeof(asset_image);

	medusa_image_assign_buffers(asset, u8_block);

	reader(asset, source);
	return 0;
}
