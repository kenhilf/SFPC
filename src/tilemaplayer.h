#ifndef _tilemaplayer_h_
#define _tilemaplayer_h_

#include "core.h"
#include "array2d.h"
class Tileset;

class TileMapLayer
{
public:
	TileMapLayer(const int width = 0, const int height = 0);
	~TileMapLayer();
	void SetTileSet(Tileset* ts);
	Tileset* GetTileSet();
	void SetTile(const int value, const int x, const int y);
	int GetTile(const int x, const int y);
	void Reset(const int width, const int height);
	int GetWidth() const;
	int GetHeight() const;

private:
	int m_width, m_height;

	Tileset* m_pTileset;
	Array2d<int, int> m_tileIndex;
};

#endif //_tilemaplayer_h_