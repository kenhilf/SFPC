#ifndef _gamewindow_h_
#define _gamewindow_h_

#include <assert.h>
#include "gamefont.h"
#include "gameobject.h"
#include "gamestring.h"
#include "spellset.h"
#include "itemset.h"
#include "gametimer.h"
class Texture;
class Slider;

class GameWindow : public GameObject
{
public:
	GameWindow(const int width = 100, const int height = 50, const Vector2f& origin = Vector2f(0.0f, 0.0f));
	virtual ~GameWindow();
	virtual void DoUpdate(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);
	virtual void ProcessInput();
	virtual void SetText(const std::string& str);
	void SetTextOffset(const int x, const int y);
	void SetSize(const int width, const int height);
	void SetFont(GameFont* pgf);
	void IgnoreSlider(const bool toggle);
	void DrawFrame(DrawMgr& drawMgr);
	GameString& GetGameString();
	Slider& GetSlider();
	void SnapToSlider();
	void SetSlideDest(const CompassDir::Type dir = CompassDir::S);
	void SetDecal(const Texture& decal);
	void SetDecalPos(const Position& pos = Position(8, 8));
	void MoveDecal(const int xdir, const int ydir);

protected:
	int m_width, m_height;
	int m_bodyWidth, m_bodyHeight;
	bool m_bIgnoreSlider;
	GameString m_winText;
	Slider m_slider;
	Texture m_decal;
	Position m_decalPos;
};

class DialogueWindow : public GameWindow
{
public:
	DialogueWindow(const int width = 304, const int height = 60, const Vector2f& origin = Vector2f(8.0f, 175.0f));
	virtual ~DialogueWindow();
	virtual void DoUpdate(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);
	virtual void ProcessInput();
	virtual void SetText(const std::string& str);
	void ResetStringPositions();
	bool AtEndOfString();

private:
	GameTimer::Tracker m_letterTracker;
	bool m_bWaitingForInput;
	bool m_bInputReceived;
	bool m_bWasOffScreen;
};

class AutoAdvanceWindow : public GameWindow
{
public:
	AutoAdvanceWindow(const int width = 296, const int height = 48, const Vector2f& origin = Vector2f(16.0f, 183.0f));
	virtual ~AutoAdvanceWindow();
	virtual void DoUpdate(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);
	virtual void ProcessInput();
	virtual void SetText(const std::string& str);
	void ResetStringPositions();
	bool AtEndOfString() const;
	bool IsExpired() const;

private:
	GameTimer::Tracker m_letterTracker;
	GameTimer::Tracker m_expireTracker;
	bool m_bDoubleTextSpeed;
	bool m_bExpired;
};

class GameWindowTexMgr
{
public:
	// returns singleton instance
	static GameWindowTexMgr& Instance() { static GameWindowTexMgr instance;  return instance; };
	void Init();

	const Texture& GetWinBody() const;
	const Texture& GetCorner() const;
	const Texture& GetTopBar() const;
	const Texture& GetSideBar() const;
	const Texture& GetBlinker() const;
	const Texture& GetBracket() const;
	const Texture& GetSpellIcon(const SpellName::Type icon) const;
	const Texture& GetItemIcon(const ItemGfxID::Type icon) const;
	int GetBorderOffset() const;

private:
	GameWindowTexMgr();
	GameWindowTexMgr(const GameWindowTexMgr& rhs);
	GameWindowTexMgr& operator=(const GameWindowTexMgr& rhs);

	Texture m_body;
	Texture m_corner;
	Texture m_sidebar;
	Texture m_topbar;
	Texture m_blinker;
	Texture m_bracket;
	SpellSet m_spellset;
	ItemSet m_itemset;
};

#endif // _gamewindow_h_

