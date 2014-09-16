#include "tileset.h"
#include "filepathmgr.h"

Tileset::Tileset() :
	m_textures(NULL),
	m_numTiles(0),
	m_width(0),
	m_height(0),
	m_tileDataWidth(0),
	m_tileDataHeight(0)
{

}

Tileset::Tileset(const std::string& filename, const int width, const int height, const int tilesize) :
	m_textures(NULL),
	m_numTiles(0),
	m_width(0),
	m_height(0),
	m_tileDataWidth(0),
	m_tileDataHeight(0)
{
	LoadTiles(filename, width, height, tilesize);
}

Tileset::~Tileset()
{
	if (m_textures != NULL)
	{
		delete[] m_textures;
	}
}

void Tileset::LoadTiles(const std::string& filename, const int width, const int height, const int tilesize)
{
	if (m_textures != NULL)
	{
		delete[] m_textures;
	}
	m_numTiles = width * height;
	m_textures = new Texture[m_numTiles];
	m_width = width;
	m_height = height;
	m_filename = filename;

	Image imgTileset;
	Image imgTileBuffer;

	imgTileset.LoadBMP(FilePathMgr::Instance().GetTilePath() + m_filename);

	m_tileDataWidth = tilesize;
	m_tileDataHeight = tilesize;

	int k = 0;

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			m_textures[k].LoadSubImage(imgTileset, (i*m_tileDataWidth), (j*m_tileDataHeight), m_tileDataWidth, m_tileDataHeight);
			k++;
		}
	}
}

const Texture& Tileset::GetTile(const int tilenum) const
{
	return m_textures[tilenum];
}

int Tileset::GetTileDataWidth() const
{
	return m_tileDataWidth;
}

int Tileset::GetTileDataHeight() const
{
	return m_tileDataHeight;
}

int Tileset::GetNumTiles() const
{
	return m_numTiles;
}

int Tileset::GetNumRows() const
{
	return m_height;
}

int Tileset::GetRowWidth() const
{
	return m_width;
}

const std::string& Tileset::GetFilename() const
{
	return m_filename;
}