#ifndef _tileset_h_
#define _tileset_h_

#include "texture.h"
#include <string>

class Tileset
{
public:
	Tileset();
	Tileset(const std::string& filename, const int width, const int height, const int tilesize = 24);
	~Tileset();
	void LoadTiles(const std::string& filename, const int width, const int height, const int tilesize = 24);
	const Texture& GetTile(const int tilenum) const;
	int GetTileDataWidth() const;
	int GetTileDataHeight() const;
	int GetNumTiles() const;
	int GetNumRows() const;
	int GetRowWidth() const;
	const std::string& GetFilename() const;

private:
	Texture* m_textures;
	std::string m_filename;
	int m_numTiles;
	int m_width;
	int m_height;
	int m_tileDataWidth;
	int m_tileDataHeight;
};

#endif //_tileset_h_