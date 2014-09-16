#include "vector2f.h"

Vector2f DirToVector2f(const Direction::Type dir)
{
	switch(dir)
	{
		case Direction::Up:    return Vector2f(0.0f, -1.0f);
		case Direction::Down:  return Vector2f(0.0f, 1.0f);
		case Direction::Left:  return Vector2f(-1.0f, 0.0f);
		case Direction::Right: return Vector2f(1.0f, 0.0f);
		case Direction::None:  return Vector2f(0.0f, 0.0f);
	}

	assert(false && "Invalid direction passed to DirToVector2f");
	return Vector2f(0.0f, 0.0f);
}

Direction::Type Vector2fToDir(const Vector2f& vec)
{
	if (vec.y < 0.0f)
		return Direction::Up;
	else if (vec.y > 0.0f)
		return Direction::Down;
	else if (vec.x < 0.0f)
		return Direction::Left;
	else if (vec.x > 0.0f)
		return Direction::Right;
	else // vec == (0, 0)
		return Direction::None;
}