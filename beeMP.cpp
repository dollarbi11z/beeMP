#include "beeMP.h"
#include "string"

bool beeMP::_flgFlipVert = false;

inline void beeMP::set_flip_image_vert()
{
	_flgFlipVert = true;
}

inline void beeMP::error_log(const uint8_t* message)
{ 
	std::cout << "[beeMP] [ERROR] " << message << std::endl;
}

inline void beeMP::debug_log( const uint8_t* message )
{
	std::cout << "[beeMP] [DEBUG] " << message << std::endl;
}

bool beeMP::sanitise_header_file( BITMAPFILEHEADER* header )
{
	// Check if it is a valid bmp file.
	if( header->bfMagic != BITMAP_MAGIC_IDENTIFIER )
	{
		error_log( "** INVALID MAGIC IDENTIFIER ** ");
		return false;
	}

	debug_log( "** VERIFIED FILE HEADER **");
	return true;
}

bool beeMP::sanitise_info_header( BITMAPINFOHEADER* header )
{
	if( header->biSize != 40 )
	{
		error_log( "INFO HEADER IS INVALID SIZE (EXPECTED: 40-BYTES)" );
		return false;
	}

	if( header->biBitCount != 24)
	{
		error_log( "INVALID BIT COUNT (EXPECTED: 24)" );
		return false;
	}

	if( header->biCompression != 0 )
	{
		error_log( "INVALID COMPRESSION VALUE (EXPECTED: 0)" );
		return false;
	}

	if( header->biHeight < 0 )
	{
		error_log( "INVALID HEIGHT VALUE (TOP-DOWN FILES ARE NOT SUPPORTED YET)" );
		return false;
	}

	if( header->biPlanes != 1 )
	{
		error_log( "INVALID COLOUR PLANE VALUE (EXPECTED: 1)" );
		return false;
	}

	if( header->biClrUsed != 0 )
	{
		error_log( "INVALID COLOUR AMOUNT (24-BIT FILES DON'T HAVE A PALETTE)" );
		return false;
	}

	debug_log( "** VERIFIED INFO HEADER **");
	return true;
}

inline int32_t beeMP::get_padding_size( BITMAPINFOHEADER& header )
{
	return ((header.biWidth * header.biBitCount + 31) / 32) * 4;
}

uint8_t* beeMP::load_bitmap( const char* file, uint32_t& width, uint32_t& height )
{
	BITMAPFILEHEADER		bmpHeader;
	BITMAPINFOHEADER		bmpInfo;

	uint8_t* ret = nullptr;

	debug_log( "** LOADING 24-BIT IMAGE **" );

	// ifstream to read image.
	std::ifstream _imageFile( file, std::ios::binary );
	std::vector<uint8_t> data;

	// Ifstream object with open mode set to 'Binary'.
	_imageFile.exceptions( std::ios::badbit | std::ios::failbit );

	if( !_imageFile )
	{
		error_log( "** UNABLE TO OPEN IMAGE-FILE **" );
		return {};
	}

	// Read in BITMAPFILEHEADER data
	_imageFile.read( (char*) &bmpHeader, sizeof( BITMAPFILEHEADER ) );

	if( sanitise_header_file( &bmpHeader ) != true )
	{
		error_log( "** FAILED TO VERIFY FILE HEADER **" );
		return {};
	}

	// Read in BITMAPINFOHEADER data
	_imageFile.read( (char*) &bmpInfo, sizeof( BITMAPINFOHEADER ) );

	if( sanitise_info_header( &bmpInfo ) != true )
	{
		error_log( "** FAILED TO VERIFY INFO HEADER **" );
		return {};
	}

	int32_t paddingRow = get_padding_size( bmpInfo );
	int32_t imageSize = bmpInfo.biHeight * paddingRow;

	_imageFile.seekg( bmpHeader.bfOffsetData, std::ios::beg );
	data.resize( imageSize );

	_imageFile.read( (char*)data.data(), data.size() );
	data.reserve( (bmpInfo.biWidth * bmpInfo.biHeight * bmpInfo.biBitCount) / 8 );

	// Fill image data 
	for( int32_t i = 0; i < bmpInfo.biHeight; i++ )
	{
		int32_t k = (bmpInfo.biHeight - 1) - i;
		uint8_t* ptr = (uint8_t*)data.data() + k * paddingRow;
		data.insert( data.end(), ptr, ptr + bmpInfo.biWidth * 3 );
	}

	// Swap BGR to RGB
	uint8_t bufRGB = 0; 
	for( uint64_t i = 0; i < bmpInfo.biSizeImage; i += 3 )
	{
		bufRGB		= data[i];
		data[i]		= data[i + 2];
		data[i + 2] = bufRGB;
	}

	width = bmpInfo.biWidth;
	height = bmpInfo.biHeight;

	ret = new uint8_t[data.size()];
	std::memcpy( ret, data.data(), data.size() );

	debug_log( "** LOADED IMAGE **" );
	return ret;
}