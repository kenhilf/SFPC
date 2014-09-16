#ifndef _vector2f_h_
#define _vector2f_h_

#include "core.h"
#include "position.h"
#include "math.h"

class Vector2f
{
public:
	Vector2f(const float newx, const float newy) : x(newx), y(newy) {}
	Vector2f(const int newx, const int newy) : x(static_cast<float>(newx)), y(static_cast<float>(newy)) {}
	Vector2f(const Position& p) : x(static_cast<float>(p.x)), y(static_cast<float>(p.y)) {}
	Vector2f(const Direction::Type dir)
	{
		switch(dir)
		{
			case Direction::Up:
				x = 0.0f;  y = -1.0f;
			break;
			case Direction::Right:
				x = 1.0f;  y = 0.0f;
			break;
			case Direction::Down:
				x = 0.0f;  y = 1.0f;
			break;
			case Direction::Left:
				x = -1.0f; y = 0.0f;
			break;
			default:
				assert(false && "Error creating a Vector2f by Direction::Type, invalid direction!");
			break;
		}
	}
	Vector2f() : x(0.0f), y(0.0f) {}

	float x, y;

	bool operator!=(const Vector2f& rhs)
	{
		if ((x != rhs.x) || (y != rhs.y))
			return true;
		else
			return false;
	}

	bool operator==(const Vector2f& rhs)
	{
		if ((x == rhs.x) && (y == rhs.y))
			return true;
		else
			return false;
	}

	void operator+=(const Vector2f& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}

	void operator-=(const Vector2f& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
	}

	// double check this (myers)
	Vector2f& operator*=(const float rhs)
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}

	bool IsZero() const
	{
		if ((x == 0.0f) && (y == 0.0f))
			return true;
		else
			return false;
	}

	float Magnitude() const
	{
		return sqrt((x * x) + (y * y));
	}

	void Normalize()
	{
		float magSquared = (x * x) + (y * y);
		if (magSquared > 0.0f)
		{
			float oneOverMag = 1.0f / sqrt(magSquared);
			x *= oneOverMag;
			y *= oneOverMag;
		}
	}

	Position CoordToTileNum() const
	{
		//@TEST: testing without the rounding up to fix Actor::DoPhyiscs - 6/13/11
//		return Position((static_cast<int>(x + 0.5f)/TILE_WIDTH), 
//						(static_cast<int>(y + 0.5f)/TILE_HEIGHT));
		return Position((static_cast<int>(x)/TILE_WIDTH), 
						(static_cast<int>(y)/TILE_HEIGHT));
	}
};

inline Vector2f operator+(const Vector2f& lhs, const Vector2f& rhs)
{
	return Vector2f(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline Vector2f operator-(const Vector2f& lhs, const Vector2f& rhs)
{
	return Vector2f(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline Vector2f operator*(const Vector2f& lhs, const float rhs)
{
	return Vector2f(lhs.x * rhs, lhs.y * rhs);
}

Vector2f DirToVector2f(const Direction::Type dir);
Direction::Type Vector2fToDir(const Vector2f& vec);

#endif //_vector2f_h_

