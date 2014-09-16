#ifndef _Vector2i_h_
#define _Vector2i_h_

#include "core.h"
#include "vector2f.h"

class Vector2i
{
public:
	Vector2i(const int newx, const int newy) { x = newx; y = newy;}
	Vector2i() { x = y = 0; }
	Vector2i(const Vector2f& v2f) { x = static_cast<int>(v2f.x + 0.5f); y = static_cast<int>(v2f.y + 0.5f); }

	int x, y;

	void operator+=(const Vector2i& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}

	bool operator==(const Vector2i& rhs)
	{
		if ((x == rhs.x) && (y == rhs.y))
			return true;
		else
			return false;
	}

	bool operator!=(const Vector2i& rhs)
	{
		if ((x != rhs.x) || (y != rhs.y))
			return true;
		else
			return false;
	}

	Vector2i CoordToTileNum() const
	{
		return Vector2i(x/TILE_WIDTH_I, y/TILE_HEIGHT_I);
	}

	Vector2i TileNumToCoord() const
	{
		return Vector2i(x*TILE_WIDTH_I, y*TILE_HEIGHT_I);
	}
};

inline Vector2i operator+(const Vector2i& lhs, const Vector2i& rhs)
{
	return Vector2i(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline Vector2i operator-(const Vector2i& lhs, const Vector2i& rhs)
{
	return Vector2i(lhs.x - rhs.x, lhs.y - rhs.y);
}


#endif //_Vector2i_h_

