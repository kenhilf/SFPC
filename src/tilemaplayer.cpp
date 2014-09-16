#include "tilemaplayer.h"
#include "tileset.h"

TileMapLayer::TileMapLayer(const int width, const int height) :
	m_width(width),
	m_height(height),
	m_pTileset(NULL)
{
	m_tileIndex.Reset(m_width, m_height);
}

TileMapLayer::~TileMapLayer()
{

}

void TileMapLayer::SetTileSet(Tileset* ts)
{
	m_pTileset = ts;
}

Tileset* TileMapLayer::GetTileSet()
{
	return m_pTileset;
}

void TileMapLayer::SetTile(const int value, const int x, const int y)
{
	assert(value < m_pTileset->GetNumTiles());
	// 8/11/11 - added safety check to only update tiles in range in order to support brushes
	// larger than 1 tile near the edge of the map.  Only checking to the right and down, as
	// the brush is anchored to the top left corner and should not be able to get to indicies 
	// less than 0.
	if ((x < m_width) && (y < m_height))
		m_tileIndex(x, y) = value;
}

int TileMapLayer::GetTile(const int x, const int y)
{
	// 8/11/11 - now returns 0 for out of bounds queries in order to support brushes
	if ((x < m_width) && (y < m_height))
		return m_tileIndex(x, y);
	else
		return 0;
}

void TileMapLayer::Reset(const int width, const int height)
{
	m_width = width;
	m_height = height;
	m_tileIndex.Reset(width, height);
}

int TileMapLayer::GetWidth() const
{
	return m_width;
}

int TileMapLayer::GetHeight() const
{
	return m_height;
}