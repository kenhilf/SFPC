#include "image.h"
#include "bmp.h"
#include <assert.h>

Image::Image() :
	m_width(0),
	m_height(0),
	m_size(0),
	m_bpp(32),
	m_pImg(NULL)
{

}

Image::~Image()
{
	if (m_pImg != NULL)
	{
		delete[] m_pImg;
	}
}

void Image::LoadBMP(const std::string& filename, const bool gradient)
{
	AUX_RGBImageRec* tempImg;
	tempImg=auxDIBImageLoad(filename.c_str());
	tempImg->Flip();

	m_width = tempImg->sizeX;
	m_height = tempImg->sizeY;
	m_size = m_width * m_height;
	int sizeInBytes = m_size * 3;
	m_bpp = 32;
	m_pImg = new Pixel32[m_size];

	Pixel32 TranspColor;
	TranspColor.r = 255;
	TranspColor.g = 0;
	TranspColor.b = 255;
	TranspColor.a = 255;

	int j = 0;
	for (int i = 0; i < sizeInBytes; i+=3)
	{
		m_pImg[j].r = tempImg->data[i];
		m_pImg[j].g = tempImg->data[i+1];
		m_pImg[j].b = tempImg->data[i+2];
		
		if (!gradient)
			m_pImg[j].a = 255;
		else //gradient
			m_pImg[j].a = 128 + ((i/3) / m_height);

		if ((m_pImg[j].r == TranspColor.r) &&
			(m_pImg[j].g == TranspColor.g) &&
			(m_pImg[j].b == TranspColor.b))
		{
			m_pImg[j].a = 0;
		}

		j++;
	}

	// clean up
	if (tempImg != NULL)
	{
		if (tempImg->data)
		{
			free(tempImg->data);
		}
		free(tempImg);
	}
}

void Image::CreateSubImage(const Image& source, const int x, const int y, 
						   const int subimgWidth, const int subimgHeight, 
						   const int bufferWidth, const int bufferHeight)
{
	assert(source.m_pImg != NULL);
	m_bpp = source.m_bpp;
	m_width = bufferWidth;
	m_height = bufferHeight;
	m_size = bufferWidth * bufferHeight;
	if (m_pImg != NULL)
	{
		delete[] m_pImg;
	}
	m_pImg = new Pixel32[m_size];

	for (int j = 0; j < bufferHeight; j++)
	{
		for (int i = 0; i < bufferWidth; i++)
		{
			m_pImg[i+(j*bufferWidth)].r = 255;
			m_pImg[i+(j*bufferWidth)].g = 0;
			m_pImg[i+(j*bufferWidth)].b = 255;
			m_pImg[i+(j*bufferWidth)].a = 0;
		}
	}

	int destIndex = 0;
	int srcIndex = 0;

	for (int j = 0; j < subimgHeight; j++)
	{
		for (int i = 0; i < subimgWidth; i++)
		{
			destIndex = i+(j*bufferWidth);
			srcIndex = (i+x)+((j+y)*source.m_width);
			assert(srcIndex <= source.m_size);
			m_pImg[destIndex] = source.m_pImg[srcIndex];
		}
	}

	// For subimages, we duplicate the last column and row to fix texture sampling precision errors on
	// the right and bottom edges of our texture.	

    // Duplicate last column
    if (subimgWidth > 1 && subimgWidth < bufferWidth)
    {
        Pixel32* pCurrPixel = &m_pImg[subimgWidth];

        for (int i = 0; i < subimgHeight; i++)
        {
            pCurrPixel[0] = pCurrPixel[-1];
            pCurrPixel += bufferWidth;
        }
    }

    // Duplicate last row
    if (subimgHeight > 1 && subimgHeight < bufferHeight)
    {
        memcpy(&m_pImg[bufferWidth * subimgHeight], &m_pImg[bufferWidth * (subimgHeight - 1)], (subimgWidth + 1) * sizeof(Pixel32));
    }
}