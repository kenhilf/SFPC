#include "line.h"

Line::Line()
{
	m_x1 = 0;
	m_y1 = 0;
	m_x2 = 0;
	m_y2 = 0;
	
	int x2 = 0;
	int y2 = 0;

	if (m_x2 >= m_x1)
	{
		m_dx = x2 - m_x1;
		m_incx = 1;
	}
	else
	{
		m_dx = m_x1 - m_x2;
		m_incx = -1;
	}

	if (m_y2 >= m_y1)
	{
		m_dy = m_y2 - m_y1;
		m_incy = 1;
	}
	else
	{
		m_dy = m_y1 - m_y2;
		m_incy = -1;
	}

	m_x = m_x1;
	m_y = m_y1;

	if (m_dx >= m_dy)
	{
		m_dy <<= 1;
		m_balance = m_dy - m_dx;
		m_dx <<= 1;
	}
	else
	{
		m_dx <<= 1;
		m_balance = m_dx - m_dy;
		m_dy <<= 1;
	}
}

Line::Line(int x1, int y1, int x2, int y2)
{
	m_x1 = x1;
	m_y1 = y1;
	m_x2 = x2;
	m_y2 = y2;
	
	if (m_x2 >= m_x1)
	{
		m_dx = x2 - m_x1;
		m_incx = 1;
	}
	else
	{
		m_dx = m_x1 - m_x2;
		m_incx = -1;
	}

	if (m_y2 >= m_y1)
	{
		m_dy = m_y2 - m_y1;
		m_incy = 1;
	}
	else
	{
		m_dy = m_y1 - m_y2;
		m_incy = -1;
	}

	m_x = m_x1;
	m_y = m_y1;

	if (m_dx >= m_dy)
	{
		m_dy <<= 1;
		m_balance = m_dy - m_dx;
		m_dx <<= 1;
	}
	else
	{
		m_dx <<= 1;
		m_balance = m_dx - m_dy;
		m_dy <<= 1;
	}
}

bool Line::GetNextPoint(int& x, int& y)
{
	x = m_x;
	y = m_y;

	bool bKeepGoing = !((m_x == m_x2) && (m_y == m_y2));

	if (m_dx >= m_dy)
	{
		if (m_balance >= 0)
		{
			m_y += m_incy;
			m_balance -= m_dx;
		}
		m_balance += m_dy;
		m_x += m_incx;
	}
	else
	{
		if (m_balance >= 0)
		{
			m_x += m_incx;
			m_balance -= m_dy;
		}
		m_balance += m_dx;
		m_y += m_incy;
	}

	return bKeepGoing;
}

