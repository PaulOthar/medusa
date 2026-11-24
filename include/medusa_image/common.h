#ifndef MEDUSA_IMAGE_COMMON
#define MEDUSA_IMAGE_COMMON

#include <stdint.h>

typedef struct /*metadata_image*/{
	uint16_t width;
	uint16_t height;
	uint8_t bits_per_pixel;

	uint8_t reserved;
	uint16_t palette_entries;//it is 16 bits in order to support the value 256 for 8bpp
}metadata_image;

typedef struct /*asset_image*/{
	metadata_image meta;

	void* palette; void* data;
	int palette_size; int effective_palette_size;
	int data_size; int effective_data_size;
}asset_image;

enum medusa_image_io_code{
	IMAGE_COMMON_CODE_GENERIC_ERROR	=	1,
	IMAGE_COMMON_CODE_MISSING_DATA	=	2,
	IMAGE_COMMON_CODE_INVALID_DATA	=	3,
	IMAGE_COMMON_CODE_MISSING_PALT	=	4,
	IMAGE_COMMON_CODE_INVALID_PALT	=	5,

	IMAGE_COMMON_CODE_INVALID_SIZE	=	6
};

//---------------------------------------------------------------------------------------------------
// Metadata [Calculation & Validation]
//---------------------------------------------------------------------------------------------------

/**
 * Calculates the total size in pixels of the image (width * height).
 * @fn int medusa_image_calc_image_size(asset_image*)
 * @param asset
 * @return size in pixels of the image
 */
int medusa_image_calc_image_size(asset_image* asset);

/**
 * Updates the effective data and palette sizes, according to metadata.
 * @fn void medusa_image_update_effective_sizes(asset_image*)
 * @param asset
 */
void medusa_image_update_effective_sizes(asset_image* asset);

/**
 * Verifies if the allocated buffers have enough space to store the image.
 * @fn int medusa_image_validate_buffers(asset_image*)
 * @param asset
 * @return A code if something is wrong, 0 otherwise
 */
int medusa_image_validate_buffers(asset_image* asset);

/**
 * Verifies if one asset could in theory store the contents of another asset
 * @fn int medusa_image_validate_equivalent(asset_image*, asset_image*)
 * @param ref
 * @param eq
 * @return A code if something is wrong, 0 otherwise
 */
int medusa_image_validate_equivalent(asset_image* ref, asset_image* eq);

//---------------------------------------------------------------------------------------------------
// Asset data [Read & Write]
//---------------------------------------------------------------------------------------------------

/**
 * Initializes the data buffer field in an asset.
 * @fn void medusa_image_init_data_buffer(asset_image*, void*, int)
 * @param asset
 * @param buffer pre-allocated buffer
 * @param size size of the buffer in bytes
 */
void medusa_image_init_data_buffer(asset_image* asset, void* buffer, int size);

/**
 * Initializes the palette buffer field in an asset.
 * @fn void medusa_image_init_palette_buffer(asset_image*, void*, int)
 * @param asset
 * @param buffer pre-allocated buffer
 * @param size size of the buffer in bytes
 */
void medusa_image_init_palette_buffer(asset_image* asset, void* buffer, int size);

/**
 * Initializes every field in an asset to their respective values.
 * Also sets the data and palette buffer to NULL.
 * @fn void medusa_image_init_asset(asset_image*, int, int, int)
 * @param asset
 * @param width image width in pixels
 * @param height image height in pixels
 * @param bits_per_pixel
 */
void medusa_image_init_asset(asset_image* asset, int width, int height, int bits_per_pixel);

/**
 * Initializes both data and palette using the specified memory block.
 * If block is null, it returns the size in bytes needed.
 * @fn int medusa_image_assign_buffers(asset_image*, void*)
 * @param asset
 * @param block pre-allocated memory block, or null.
 * @return block ? 0 : <size in bytes>
 */
int medusa_image_assign_buffers(asset_image* asset, void* block);

/**
 * Creates and initializes a asset from specified memory block.
 * Block address points to the created asset_image.
 * If block is null, it returns the size in bytes needed.
 * @fn int medusa_image_create_asset(int, int, int, void*)
 * @param width image width in pixels
 * @param height image height in pixels
 * @param bits_per_pixel
 * @param block pre-allocated memory block, or null.
 * @return block ? 0 : <size in bytes>
 */
int medusa_image_create_asset(int width, int height, int bits_per_pixel, void* block);

//---------------------------------------------------------------------------------------------------
// Pixel data [Read & Write]
//---------------------------------------------------------------------------------------------------

typedef uint32_t (*medusa_image_get_function)(asset_image*, int);
typedef uint32_t (*medusa_image_set_function)(asset_image*, int, uint32_t);

medusa_image_get_function medusa_image_function_get_palette_index(int bpp);
uint32_t medusa_image_get_palette_index(asset_image* asset, int pos);
uint32_t medusa_image_get_palette_index_8bpp(asset_image* asset, int pos);
uint32_t medusa_image_get_palette_index_4bpp(asset_image* asset, int pos);
uint32_t medusa_image_get_palette_index_1bpp(asset_image* asset, int pos);

medusa_image_set_function medusa_image_function_set_palette_index(int bpp);

uint32_t medusa_image_set_palette_index(asset_image* asset, int pos, uint32_t index);
uint32_t medusa_image_set_palette_index_8bpp(asset_image* asset, int pos, uint32_t index);
uint32_t medusa_image_set_palette_index_4bpp(asset_image* asset, int pos, uint32_t index);
uint32_t medusa_image_set_palette_index_1bpp(asset_image* asset, int pos, uint32_t index);

medusa_image_get_function medusa_image_function_get_pixel(int bpp);

uint32_t medusa_image_get_pixel(asset_image* asset, int pos);
uint32_t medusa_image_get_pixel_32bpp(asset_image* asset, int pos);
uint32_t medusa_image_get_pixel_24bpp(asset_image* asset, int pos);
uint32_t medusa_image_get_pixel_8bpp(asset_image* asset, int pos);
uint32_t medusa_image_get_pixel_4bpp(asset_image* asset, int pos);
uint32_t medusa_image_get_pixel_1bpp(asset_image* asset, int pos);

medusa_image_set_function medusa_image_function_set_pixel(int bpp);

uint32_t medusa_image_set_pixel(asset_image* asset, int pos, uint32_t color);
uint32_t medusa_image_set_pixel_32bpp(asset_image* asset, int pos, uint32_t color);
uint32_t medusa_image_set_pixel_24bpp(asset_image* asset, int pos, uint32_t color);

#endif
