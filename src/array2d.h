#ifndef _array2d_h_
#define _array2d_h_

#include "core.h"

// Efficient 2d array class, data is stored in row major order
// (i.e. x values from first row, followed by x values from second
// row, etc.)
// Courtesy of Tony!
template <typename T, typename SizeType = size_t>
class Array2d
{
public:
	typedef Array2d ThisType;

	Array2d() : m_pArray(0) { }
	Array2d(SizeType sizeX, SizeType sizeY) : m_pArray(0) { Reset(sizeX, sizeY); }
	~Array2d() { Reset(); }

	void Reset(SizeType sizeX = 0, SizeType sizeY = 0)
	{
		m_SizeX = sizeX;
		m_SizeY = sizeY;

		delete [] m_pArray;

		if (sizeX > 0 && sizeY > 0)
		{
			m_pArray = new T[sizeX * sizeY];
		}
		else
		{
			ASSERT(sizeX == 0 && sizeY == 0);
			m_pArray = NULL;
		}
	}

	SizeType Size() const { return m_SizeX * m_SizeY; }
	SizeType SizeX() const { return m_SizeX; }
	SizeType SizeY() const { return m_SizeY; }

	const T& operator()(SizeType x, SizeType y) const
	{
		return m_pArray[y * m_SizeX + x];
	}

	T& operator()(SizeType x, SizeType y)
	{
		return m_pArray[y * m_SizeX + x];
	}

	const T& At(SizeType x, SizeType y) const
	{
		ASSERT(x < m_SizeX && y < m_SizeY);
		return m_pArray[y * m_SizeX + x];
	}

	T& At(SizeType x, SizeType y)
	{
		ASSERT(x < m_SizeX && y < m_SizeY);
		return m_pArray[y * m_SizeX + x];
	}

	const T* RawPtr() const { return m_pArray; }
	T* RawPtr() { return m_pArray; }

private:
	T* m_pArray;
	SizeType m_SizeX, m_SizeY;
};

#endif //_array2d_h_