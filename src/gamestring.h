#ifndef _gamestring_h_
#define _gamestring_h_

#include <string>
#include "gameobject.h"
#include "slider.h"
class GameFont;

class GameString : public GameObject
{
public:
	GameString(const std::string& text = "");
	virtual ~GameString();
	void SetFont(GameFont* const pgf);
	void SetString(const std::string& text);
	void SetOffset(const int x, const int y);
	void SetDrawSpace(const DrawSpace::Type ds);
	void SetScale(const float scale);
	void SetFirstChar(const int fc);
	void SetLastChar(const unsigned int lc);
	void SetTint(const float r = 1.0f, const float g = 1.0f, const float b = 1.0f);
	void SetBounds(const int x, const int y, const int width, const int height);
	void UpdateBounds(const int x, const int y, const int width, const int height);
	void ClearBounds();
	const char GetChar(const unsigned int index) const;
	const int GetFirstChar() const;
	const int GetLastChar() const;
	const int GetStringLength() const;
	void SetLineWidth(const int lw);
	void ScrollLine(const Direction::Type dir = Direction::Up, const int numlines = 1);
	const bool IsScrolling() const;
	void ResetScrolling();
	virtual void DoUpdate(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);

private:
	void DrawChar(const int chr, const float x, const float y, DrawMgr& drawMgr);
	void ProcessString();

	std::string m_string;
	GameFont* m_pgf;
	float m_scale;
	DrawSpace::Type m_drawSpace;

	int m_firstChar;
	int m_lastChar;
	int m_lineWidth;

	float m_tintRed;
	float m_tintGreen;
	float m_tintBlue;

	bool m_bHasBounds;
	int m_boundX, m_boundY, m_boundW, m_boundH;

	Slider m_slider;
	int m_offX, m_offY;
};

#endif //_gamestring_h_