#include "beeMP.h"
#include "string"

bool beeMP::_flgFlipVert = false;

uint8_t	 beeMP::_flgEndian { 0 };
uint32_t beeMP::_rowStride { 0 };

inline void beeMP::set_flip_image_vert()
{
	_flgFlipVert = true;
}

inline void beeMP::error_log(const char* message)
{ 
	std::cout << "[beeMP] [ERROR] " << message << std::endl;
}

inline void beeMP::debug_log( const char* message )
{
	std::cout << "[beeMP] [DEBUG] " << message << std::endl;
}

uint32_t beeMP::align_stride( uint32_t stride )
{
	uint32_t newStride = _rowStride;

	while( newStride % stride != 0 )
	{
		newStride++;
	}
	return newStride;
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

bool beeMP::sanitise_info_header_24( BITMAPINFOHEADER* header )
{
	if( header->biSize != 40 )
	{
		error_log( "** INFO HEADER IS INVALID SIZE (EXPECTED: 40-BYTES) **" );
		return false;
	}

	if( header->biBitCount != 24)
	{
		error_log( "** INVALID BIT COUNT (EXPECTED: 24) **" );
		return false;
	}

	if( header->biCompression != 0 )
	{
		error_log( "** INVALID COMPRESSION VALUE (EXPECTED: 0) **" );
		return false;
	}

	if( header->biHeight < 0 )
	{
		error_log( "** INVALID HEIGHT VALUE (TOP-DOWN FILES ARE NOT SUPPORTED YET) ** " );
		return false;
	}

	if( header->biPlanes != 1 )
	{
		error_log( "** INVALID COLOUR PLANE VALUE (EXPECTED: 1) **" );
		return false;
	}

	if( header->biClrUsed != 0 )
	{
		error_log( " ** INVALID COLOUR AMOUNT (24-BIT FILES DON'T HAVE A PALETTE) **" );
		return false;
	}

	debug_log( "** VERIFIED INFO HEADER **");
	return true;
}

bool beeMP::sanitise_info_header_32( BITMAPV4HEADER* header )
{
	return false;
}

uint8_t* beeMP::get_colour_palette_32( BITMAPINFOHEADER* info, std::ifstream& stream )
{
	uint8_t* palette32;

	debug_log( "** RETRIEVED COLOUR PALETTE **" );
	return nullptr;
}

uint8_t* beeMP::load_bitmap( const char* file, uint32_t& width, uint32_t& height, bool hasAlpha )
{
	uint8_t* ret = nullptr;

	switch( hasAlpha )
	{
		case true:
			ret = load_bitmap_32( file, width, height );
			return ret;
		case false:
			ret = load_bitmap_24( file, width, height );
			return ret;
	}
	return ret;
}

uint8_t* beeMP::load_bitmap_24( const char* file, uint32_t& width, uint32_t& height )
{
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfo;

	debug_log( "** LOADING 24-BIT IMAGE **" );

	// Store the image data.
	std::vector<uint8_t> _imageData;

	// ifstream to read image.
	std::ifstream _imageFile( file, std::ios::binary );

	// Ifstream object with open mode set to 'Binary'.
	_imageFile.exceptions( std::ios::badbit | std::ios::failbit );

	if( !_imageFile )
	{
		error_log( "** UNABLE TO OPEN IMAGE-FILE **" );
		return nullptr;
	}

	// Read in BITMAPFILEHEADER data
	_imageFile.read( (char*) &bmpHeader, sizeof( BITMAPFILEHEADER ) );

	if( sanitise_header_file( &bmpHeader ) != true )
	{
		error_log( "** FAILED TO VERIFY FILE HEADER **" );
		return nullptr;
	}

	// Read in BITMAPINFOHEADER data
	_imageFile.read( (char*) &bmpInfo, sizeof( BITMAPINFOHEADER ) );

	if( sanitise_info_header_24( &bmpInfo ) != true )
	{
		error_log( "** FAILED TO VERIFY INFO HEADER **" );
		return nullptr;
	}

	width = bmpInfo.biWidth;
	height = bmpInfo.biHeight;

	uint32_t dataSize = (bmpInfo.biBitCount * bmpInfo.biWidth + 31 / 32) * 4;
	_imageData.resize( dataSize );

	// Read image data
	_imageFile.read( (char*) _imageData.data(), dataSize);

	// Swap B and R channels
	for( uint32_t i = 0; i < dataSize; i += 3 )
	{
		uint8_t bufRGB		= _imageData[i];
		_imageData[i]		= _imageData[i + 2];
		_imageData[i + 2]	= bufRGB;
	}

	debug_log( "** LOADED IMAGE **" );

	return _imageData.data();
}

uint8_t* beeMP::load_bitmap_32( const char* file, uint32_t& width, uint32_t& height )
{
	BITMAPFILEHEADER		bmpHeader;
	BITMAPV4HEADER			bmpInfo;

	debug_log( "**	LOADING 32-BIT IMAGE  **" );

	// Store the image data.
	std::vector<uint8_t> _imageData;

	// ifstream to read image.
	std::ifstream _imageFile( file, std::ios::binary );

	// Ifstream object with open mode set to 'Binary'.
	_imageFile.exceptions( std::ios::badbit | std::ios::failbit );

	if( !_imageFile )
	{
		error_log( "** UNABLE TO OPEN IMAGE-FILE **" );
		return nullptr;
	}

	// Read in BITMAPFILEHEADER data
	_imageFile.read( (char*) &bmpHeader, sizeof( BITMAPFILEHEADER ) );

	if( sanitise_header_file( &bmpHeader ) != true )
	{
		error_log( "** FAILED TO VERIFY FILE HEADER **" );
		return nullptr;
	}

	// Read in BITMAPINFOHEADER data
	_imageFile.read( (char*) &bmpInfo, sizeof( BITMAPINFOHEADER ) );

	if( sanitise_info_header_32( &bmpInfo ) != true )
	{
		error_log( "** FAILED TO VERIFY INFO HEADER **" );
		return nullptr;
	}

	bmpInfo.bV4Size = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPV4HEADER );
	bmpHeader.bfOffsetData = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPV4HEADER );

	// Calculate the size of the data array
	uint32_t dataSize = bmpInfo.bV4Width * bmpInfo.bV4Height * bmpInfo.bV4BitCount / 8;
	_imageData.resize( dataSize );

	if( bmpInfo.bV4BitCount % 4 == 0 )
	{
		_imageFile.read( reinterpret_cast<char*>(_imageData.data()), dataSize );
		bmpHeader.bfSize += dataSize;
	}
	else
	{
		_rowStride = bmpInfo.bV4Width * bmpInfo.bV4BitCount / 8;
		uint32_t newStride = align_stride( 4 );

		std::vector<uint8_t> padding_row( newStride - _rowStride );

		for( int32_t y = 0; y < bmpInfo.bV4Height; y++ )
		{
			_imageFile.read( (char*) (_imageData.data() + _rowStride * y), _rowStride );
			_imageFile.read( (char*) padding_row.data(), padding_row.size() );
		}
		bmpHeader.bfSize += dataSize + bmpInfo.bV4Height * static_cast<uint32_t>(padding_row.size());
	}

	debug_log( "** LOADED IMAGE **" );
	return _imageData.data();
}