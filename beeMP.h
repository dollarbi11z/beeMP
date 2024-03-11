#pragma once

#pragma once

//#pragma pack(push, 1)

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <format>

/* BMP Related Macros */
#define BMP_FILE_TYPE			0x4D42
#define BMP_HEADER_SIZE			14				// Size of bmp file header in bytes.
#define BMP_INFO_SIZE			40				// Size of bmp info in bytes.
#define BMP_COMPRESSION			0

/* File Extensions */
#define BITMAP	".bmp"
#define PNG		".png"
#define JPEG	".jpeg"

/* A fast static library for loading images*/
class fastImage
{
private:
	/* Struct to contain the bitmap header data */
	typedef struct bmp_file_header_t
	{
		uint16_t	bfType;      // File type signature (should be 'BM')
		uint32_t	bfSize;      // Size of the BMP file in bytes
		uint16_t	bfReserved1; // Reserved, must be zero
		uint16_t	bfReserved2; // Reserved, must be zero
		uint32_t	bfOffsetData;   // Offset to the start of image data

	} bmp_file_header_t;

	/* Struct to contain the bitmap file info */
	typedef struct bmp_info_header_t
	{
		uint32_t	biSize;          // Size of the header (should be 40)
		uint32_t	biWidth;         // Image width in pixels
		uint32_t	biHeight;        // Image height in pixels
		uint16_t	biPlanes;        // Number of color planes (must be 1)
		uint16_t	biBitCount;      // Number of bits per pixel
		uint32_t	biCompression;   // Compression method (0 for uncompressed)
		uint32_t	biSizeImage;     // Size of the raw bitmap data
		uint32_t	biXPelsPerMeter; // Horizontal resolution (pixels per meter)
		uint32_t	biYPelsPerMeter; // Vertical resolution (pixels per meter)
		uint32_t	biClrUsed;       // Number of colors in the palette
		uint32_t	biClrImportant;  // Number of important colors (usually 0)
	} bmp_info_header_t;

	/* Struct to store the colours used */
	typedef struct bmp_colour_header_t
	{
		uint32_t bRed;
		uint32_t bBlue;
		uint32_t bGreen;
		uint32_t bAlpha;
		uint32_t bColourSpace; // sRGB 0x73524742
		uint32_t bReserved;

	} bmp_colour_header_t;

	static uint32_t _rowStride;

	static void error_log( const char* message );
	static void verify_bitmask_info( bmp_colour_header_t& colourHeader );

	static uint32_t align_stride( uint32_t stride );

public:
	static uint8_t* load_bitmap( const char* file, uint32_t& width, uint32_t& height );
};
//#pragma pack(pop)