#ifndef _texture_h_
#define _texture_h_

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "image.h"

class Texture
{
public:
	Texture();
	void LoadImage(const Image& img);
	void LoadSubImage(const Image& img, const int x, const int y, 
					  const int width, const int height);

	GLuint m_texID;
	int m_width;
	int m_height;
	int m_dataWidth;
	int m_dataHeight;

private:
	void ScaleBuffer();
	void Bind(const Image& buffer);
};

#endif //_texture_h_