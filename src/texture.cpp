#include "texture.h"
#include <assert.h>

Texture::Texture() :
	m_texID(0),
	m_width(0),
	m_height(0),
	m_dataWidth(0),
	m_dataHeight(0)
{

}

void Texture::LoadImage(const Image& img)
{
	m_dataWidth = img.m_width;
	m_dataHeight = img.m_height;
	ScaleBuffer();

	Image buff;
	buff.CreateSubImage(img, 0, 0, m_dataWidth, m_dataHeight, m_width, m_height);

	Bind(buff);
}

void Texture::LoadSubImage(const Image& img, const int x, const int y, 
						   const int width, const int height)
{
	m_dataWidth = width;
	m_dataHeight = height;
	ScaleBuffer();

	Image buff;
	buff.CreateSubImage(img, x, y, width, height, m_width, m_height);

	Bind(buff);
}

void Texture::ScaleBuffer()
{
	int size = 16;
	while ((size < m_dataWidth) || (size < m_dataHeight))
	{
		size = size*2;
		assert(size <= 1024);
	}
	m_width = m_height = size;
}

void Texture::Bind(const Image& buffer)
{
	glGenTextures(1, &m_texID);
	glBindTexture(GL_TEXTURE_2D, m_texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.m_pImg);

	// Set filtering to nearest neighbor so that we don't interpolate at all
	// (get the old-school pixelated look)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Make sure texture sampling never wraps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

