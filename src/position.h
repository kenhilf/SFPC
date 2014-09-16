#ifndef _position_h_
#define _position_h_

#include "core.h"

class Position
{
public:
	Position(int newx, int newy) { x = newx; y = newy;}
	Position() { x = y = 0; }

	int x, y;

	void operator+=(const Position& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}

	bool operator==(const Position& rhs) const
	{
		if ((x == rhs.x) && (y == rhs.y))
			return true;
		else
			return false;
	}

	bool operator!=(const Position& rhs) const
	{
		if ((x != rhs.x) || (y != rhs.y))
			return true;
		else
			return false;
	}

	Position CoordToTileNum() const
	{
		return Position(x/TILE_WIDTH, y/TILE_HEIGHT);
	}

	Position TileNumToCoord() const
	{
		return Position(x*TILE_WIDTH, y*TILE_HEIGHT);
	}
};

inline Position operator+(const Position& lhs, const Position& rhs)
{
	return Position(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline Position operator-(const Position& lhs, const Position& rhs)
{
	return Position(lhs.x - rhs.x, lhs.y - rhs.y);
}


#endif //_position_h_

