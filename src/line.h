#ifndef _line_h_
#define _line_h_

class Line
{
public:
	Line();
	Line(int x1, int y1, int x2, int y2);

	bool GetNextPoint(int& x, int& y);

private:
	int	m_x, m_y;
	int m_x1, m_y1;
	int	m_x2, m_y2;
	int	m_dx, m_dy;
	int	m_incx, m_incy;
	int	m_balance;
	int m_rate;
	
};

#endif //_line_h_


