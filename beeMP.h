/*
											$$\                           $$\      $$\ $$$$$$$\  
											$$ |                          $$$\    $$$ |$$  __$$\ 
											$$$$$$$\   $$$$$$\   $$$$$$\  $$$$\  $$$$ |$$ |  $$ |
											$$  __$$\ $$  __$$\ $$  __$$\ $$\$$\$$ $$ |$$$$$$$  |
											$$ |  $$ |$$$$$$$$ |$$$$$$$$ |$$ \$$$  $$ |$$  ____/ 
											$$ |  $$ |$$   ____|$$   ____|$$ |\$  /$$ |$$ |      
											$$$$$$$  |\$$$$$$$\ \$$$$$$$\ $$ | \_/ $$ |$$ |      
											\_______/  \_______| \_______|\__|     \__|\__|      


											HOW TO USE:				
											Create two unsigned ints in your class to store dimensions

											Create an unsigned char* in your class to store the data.

											Use beeMP::load() and pass the texture location and 
											variables in.

										    There is also a boolean value for if the image has alpha
											channels and this will determine the image to be 32-bit.

											load() will fill the input variables with the data read
											from the bitmap file.

											From there, you now have the necessary image data to map
											to an OpenGL texture or related uses.

											I may create a custom struct to contain all of the image
											data so the user can extract the data manually.
										
*/

#pragma once
#pragma pack(push, 1)

#define WIN32_LEAN_AND_MEAN 

#define BITMAP_MAGIC_IDENTIFIER	0x4D42

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
	int32_t     biHeight;        // Image height in pixels
	uint16_t    biPlanes;        // Number of color planes (must be 1)
	uint16_t    biBitCount;      // Number of bits per pixel
	uint32_t    biCompression;   // Compression method (0 for uncompressed)
	uint32_t    biSizeImage;     // Size of the raw bitmap data
	uint32_t    biXPelsPerMeter; // Horizontal resolution (pixels per meter)
	uint32_t    biYPelsPerMeter; // Vertical resolution (pixels per meter)
	uint32_t    biClrUsed;       // Number of colors in the palette
	uint32_t    biClrImportant;  // Number of important colors (usually 0)
} BITMAPINFOHEADER;

/* A fast, (hopefully) safe static library for loading 24-bit bitmap files. */
class beeMP
{
private:
	static bool                 _flgFlipVert;	/* Will the image be flipped vertically (W.I.P) */

	inline static void error_log( const uint8_t* message); /* Use this to log errors and display to console */
	inline static void debug_log( const uint8_t* message ); /* Use this to debug output and display to console */

	inline static int32_t get_padding_size( BITMAPINFOHEADER& header ); /* Returns the necessary padding */

	static bool sanitise_header_file( BITMAPFILEHEADER* header );
	static bool sanitise_info_header( BITMAPINFOHEADER* header );

public:
	static uint8_t* load_bitmap( const char* file, uint32_t& width, uint32_t& height );

	// TODO: implement row order flipping
	inline static void set_flip_image_vert();
};

#pragma pack(pop)