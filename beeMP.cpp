// beeMP.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "beeMP.h"

uint32_t fastImage::_rowStride { 0 };

void fastImage::error_log( const char* message )
{
	std::cerr << "[fastImage] [ERROR] " << message  << std::endl;
}

void fastImage::verify_bitmask_info( bmp_colour_header_t& colourHeader )
{
	bmp_colour_header_t expectedColourHeader { 0 };

	if( expectedColourHeader.bRed != colourHeader.bRed || expectedColourHeader.bBlue != colourHeader.bBlue ||
		expectedColourHeader.bGreen != colourHeader.bGreen || expectedColourHeader.bAlpha != colourHeader.bAlpha )
	{
		error_log( "Colour bitmasks do not match." );
		return;
	}
	else if( expectedColourHeader.bColourSpace != colourHeader.bColourSpace )
	{
		error_log( "Colour space doesn't match (Expected: sRGB)" );
		return;
	}
}

uint32_t fastImage::align_stride( uint32_t stride )
{
	_rowStride = stride;
	uint32_t newStride = _rowStride;

	while( newStride % 4 != 0 )
	{
		newStride++;
	}

	return newStride;
}

uint8_t* fastImage::load_bitmap( const char* file, uint32_t& width, uint32_t& height )
{
	bmp_file_header_t	bmpHeader;
	bmp_info_header_t	bmpInfo;
	bmp_colour_header_t bmpColours;

	std::vector<uint8_t> imageData;

	// Ifstream object with open mode set to 'Binary'.
	std::ifstream imageFile( file, std::ios::binary );

	// Set ifstream exceptions
	imageFile.exceptions( std::ios::badbit | std::ios::failbit );

	// Check if file exists.
	if( !imageFile )
	{
		error_log( "Unable to open file." );
		return {};
	}

	imageFile.read( reinterpret_cast<char*>(&bmpHeader), sizeof( bmp_file_header_t ) );

	if( bmpHeader.bfType == BMP_FILE_TYPE )
	{

		error_log( "File is not a valid bitmap." );
		return {};
	}

	imageFile.read( reinterpret_cast<char*>(&bmpInfo), sizeof( bmp_info_header_t ) );

	if( bmpInfo.biCompression != BMP_COMPRESSION )
	{
		error_log( "fastImage only supports compression values of 0." );
		return {};
	}

	if( bmpInfo.biSize >= (sizeof( bmp_info_header_t ) + sizeof( bmp_colour_header_t )) )
	{
		imageFile.read( reinterpret_cast<char*>(&bmpColours), sizeof( bmp_colour_header_t ) );
		verify_bitmask_info( bmpColours );
	}
	else
	{
		error_log( "File doesn't contain bitmask data." );
		return {};
	}

	imageFile.seekg( bmpHeader.bfOffsetData, std::ios::beg );

	// Check for bit count 
	switch( bmpInfo.biBitCount )
	{
		case 24:
			bmpInfo.biSize = sizeof( bmp_info_header_t );
			bmpHeader.bfOffsetData = sizeof( bmp_file_header_t ) + sizeof( bmp_info_header_t );
			break;
		case 32:
			bmpInfo.biSize = sizeof( bmp_info_header_t ) + sizeof( bmp_colour_header_t );
			bmpHeader.bfOffsetData = sizeof( bmp_file_header_t ) + sizeof( bmp_info_header_t ) + sizeof( bmp_colour_header_t );
			break;
		default:
			error_log( "** CURRENTLY (24-Bit & 32-Bit images supported) **" );
			return {};
	}

	bmpHeader.bfSize = bmpHeader.bfOffsetData;

	if( bmpInfo.biHeight < 0 )
	{
		error_log( "** Only supports bitmaps with origin in the top left corner. **" );
		return{};
	}

	imageData.resize( bmpInfo.biWidth * bmpInfo.biHeight * bmpInfo.biBitCount / 8 );

	width = bmpInfo.biWidth;
	height = bmpInfo.biHeight;

	if( bmpInfo.biBitCount % 4 == 0 )
	{
		imageFile.read( reinterpret_cast<char*>(imageData.data()), imageData.size() );
		bmpHeader.bfSize += imageData.size();
	}
	else
	{
		_rowStride = bmpInfo.biWidth * bmpInfo.biBitCount / 8;
		uint32_t newStride = align_stride( 4 );

		std::vector<uint8_t> padding_row( newStride - _rowStride );

		for( uint8_t y = 0; y < bmpInfo.biHeight; y++ )
		{
			imageFile.read( reinterpret_cast<char*>(imageData.data() + _rowStride * y), _rowStride );
			imageFile.read( reinterpret_cast<char*>(padding_row.data()), padding_row.size() );

		}
		bmpHeader.bfSize += imageData.size() + bmpInfo.biHeight * padding_row.size();
	}

	imageFile.read( reinterpret_cast<char*>(imageData.data()), bmpHeader.bfSize );
	return imageData.data();
}

