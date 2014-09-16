#ifndef _image_h_
#define _image_h_

#include <windows.h>
#include <string>
#include "core.h"

// Temporary Image class used to load bitmaps for conversion into OpenGL Textures.
class Image
{
public:
	Image();
	~Image();
	void LoadBMP(const std::string& filename, const bool gradient = false);
	void CreateSubImage(const Image& source, const int x, const int y, 
						const int subimgWidth, const int subimgHeight, 
						const int bufferWidth, const int bufferHeight);

	int m_width;
	int m_height;
	int m_size;
	int m_bpp;
	Pixel32* m_pImg;
};

#endif //_image_h_