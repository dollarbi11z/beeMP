/*
											$$\                           $$\      $$\ $$$$$$$\  
											$$ |                          $$$\    $$$ |$$  __$$\ 
											$$$$$$$\   $$$$$$\   $$$$$$\  $$$$\  $$$$ |$$ |  $$ |
											$$  __$$\ $$  __$$\ $$  __$$\ $$\$$\$$ $$ |$$$$$$$  |
											$$ |  $$ |$$$$$$$$ |$$$$$$$$ |$$ \$$$  $$ |$$  ____/ 
											$$ |  $$ |$$   ____|$$   ____|$$ |\$  /$$ |$$ |      
											$$$$$$$  |\$$$$$$$\ \$$$$$$$\ $$ | \_/ $$ |$$ |      
											\_______/  \_______| \_______|\__|     \__|\__|      


											TODO:
											- Support different bit depths (2, 4, 8, 16, 32).
											- Ensure safety of code.
											- Optimise performance.
											- Break this alot in testing.
										
*/

#pragma once
#pragma pack(push, 1)

#define WIN32_LEAN_AND_MEAN 

#define BITMAP_MAGIC_IDENTIFIER	0x4D42
#define SRGB_COLOUR_SPACE		0x73524742

#include <unordered_map>
#include <iostream>
#include <cstdint>
#include <vector>
#include <fstream>

/* Struct to contain the bitmap header data */
typedef struct tagBITMAPFILEHEADER
{
	uint16_t    bfMagic;        // Magic Identifier of File: Should be ('B', 'M')
	uint32_t    bfSize;         // Size of the BMP file in bytes
	uint32_t    bfUnused;       // Should be 0.
	uint32_t    bfOffsetData;   // Offset to the start of image data

} BITMAPFILEHEADER;

/* Struct to contain the bitmap info */
typedef struct tagBITMAPINFOHEADER
{
	uint32_t    biSize;          // Size of the header (should be 40)
	uint32_t    biWidth;         // Image width in pixels
	uint32_t    biHeight;        // Image height in pixels
	uint16_t    biPlanes;        // Number of color planes (must be 1)
	uint16_t    biBitCount;      // Number of bits per pixel
	uint32_t    biCompression;   // Compression method (0 for uncompressed)
	uint32_t    biSizeImage;     // Size of the raw bitmap data
	uint32_t    biXPelsPerMeter; // Horizontal resolution (pixels per meter)
	uint32_t    biYPelsPerMeter; // Vertical resolution (pixels per meter)
	uint32_t    biClrUsed;       // Number of colors in the palette
	uint32_t    biClrImportant;  // Number of important colors (usually 0)
} BITMAPINFOHEADER;

typedef struct tagBITMAPV4HEADER
{
	uint32_t bV4Size;
	int32_t  bV4Width;
	int32_t  bV4Height;
	uint16_t bV4Planes;
	uint16_t bV4BitCount;
	uint32_t bV4V4Compression;
	uint32_t bV4SizeImage;
	int32_t  bV4XPelsPerMeter;
	int32_t  bV4YPelsPerMeter;
	uint32_t bV4ClrUsed;
	uint32_t bV4ClrImportant;
	uint32_t bV4RedMask;
	uint32_t bV4GreenMask;
	uint32_t bV4BlueMask;
	uint32_t bV4AlphaMask;
	uint32_t bV4CSType;
	int32_t  bV4Endpoints[9];
	uint32_t bV4GammaRed;
	uint32_t bV4GammaGreen;
	uint32_t bV4GammaBlue;
} BITMAPV4HEADER;

/* A (no-longer) fast, (no-longer) simple, (hopefully) safe static library for loading 24-bit & 32-bit bitmap files. */
class beeMP
{
private:
	static uint32_t             _rowStride;		

	static uint8_t              _flgEndian;		/* 1u - Little Endian | 2u - Big Endian */ 
	static bool                 _flgFlipVert;	/* Will the image be flipped vertically (W.I.P) */

	inline static void error_log( const char* message); /* Use this to log errors and display to console */
	inline static void debug_log( const char* message ); /* Use this to debug output and display to console */

	static bool sanitise_header_file( BITMAPFILEHEADER* header );
	static bool sanitise_info_header_24( BITMAPINFOHEADER* header );
	static bool sanitise_info_header_32( BITMAPV4HEADER* header );

	static uint32_t align_stride( uint32_t stride );
	static uint8_t* get_colour_palette_32( BITMAPINFOHEADER* info, std::ifstream& stream );

	/* BROKEN */
	static uint8_t* load_bitmap_24( const char* file, uint32_t& width, uint32_t& height );

	/* NEEDS IMPLEMENTATION */
	static uint8_t* load_bitmap_32( const char* file, uint32_t& width, uint32_t& height );

public:
	static uint8_t* load_bitmap( const char* file, uint32_t& width, uint32_t& height, bool hasAlpha );

	// TODO: implement row order flipping
	inline static void set_flip_image_vert();
};

#pragma pack(pop)