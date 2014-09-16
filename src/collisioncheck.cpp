#include "collisioncheck.h"
#include "core.h"
#include "vector2f.h"
#include <cmath>

// assuming all entities checked are TILE_WIDTH wide and TILE_HEIGHT tall.
bool CollisionCheck(const Vector2f& v1, const Vector2f& v2, Vector2f& offset)
{
	float sX1 = v1.x;
	float sX2 = v1.x + static_cast<float>(TILE_WIDTH);
	float dX1 = v2.x;
	float dX2 = v2.x + static_cast<float>(TILE_WIDTH);

	float sY1 = v1.y;
	float sY2 = v1.y + static_cast<float>(TILE_HEIGHT);
	float dY1 = v2.y;
	float dY2 = v2.y + static_cast<float>(TILE_HEIGHT);

	// easier to check if two rects don't collide than to check if they do
	if ((sX2 <= dX1 || sX1 >= dX2) ||
		(sY2 <= dY1 || sY1 >= dY2)) 
	{
		// they don't overlap
		return false;
	}

	// if they don't not-overlap, calculate the offset to un-overlap them by finding
	// the smaller of the two sets of offsets
	float xOff1 = dX1 - sX2;
	float xOff2 = dX2 - sX1;
	if (-xOff1 < xOff2)
		offset.x = xOff1;
	else
		offset.x = xOff2;

	float yOff1 = dY1 - sY2;
	float yOff2 = dY2 - sY1;
	if (-yOff1 < yOff2)
		offset.y = yOff1;
	else
		offset.y = yOff2;

	// return the smaller of the two offsets, or both if equal
	if (abs(offset.x) < abs(offset.y))
		offset.y = 0;
	else if (abs(offset.y) < abs(offset.x))
		offset.x = 0;

	return true;
}
