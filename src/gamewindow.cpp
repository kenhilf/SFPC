#include "gamewindow.h"
#include "gameobjectmgr.h"
#include "filepathmgr.h"
#include "inputmgr.h"
#include "texture.h"
#include "slider.h"

//////////////////////
// GameWindow		//
//////////////////////

GameWindow::GameWindow(const int width, const int height, const Vector2f& origin) : 
	GameObject(LayerSpace::Window),
	m_bIgnoreSlider(false),
	m_decalPos(8, 8)
{
	SetSize(width, height);

	SetLocalPos(origin);

	m_winText.SetOffset(8, 8);
	AddChild(m_winText);

	m_slider = Slider(origin);
	m_slider.SetSlideRate(1200);
	SetSlideDest(CompassDir::S);
	m_slider.SnapOffScreen();
	SnapToSlider();
	AddChild(m_slider);
}

GameWindow::~GameWindow()
{

}

void GameWindow::DoUpdate(const float ticks)
{
	if (!m_bIgnoreSlider)
	{
		SnapToSlider();
	}

	Vector2f pos = m_slider.GetLocalPos();
	m_winText.UpdateBounds(static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f), m_width, m_height);
}

void GameWindow::DoDraw(DrawMgr& drawMgr)
{
	DrawFrame(drawMgr);

	// The text of the window is a child of this window
	// and will be drawn during its own DoDraw pass.
}

void GameWindow::ProcessInput()
{
	if (InputMgr::Instance().WasPressed(GEN_A) &&
		GameObjectMgr::Instance().CheckCurrentFocus(this))
	{
		GameObjectMgr::Instance().PopFocusObject();
		m_slider.SlideOffScreen();
	}
}

void GameWindow::SetText(const std::string& str)
{
	m_winText.SetString(str);
}

void GameWindow::SetTextOffset(const int x, const int y)
{
	m_winText.SetOffset(x, y);
}

void GameWindow::SetSize(const int width, const int height)
{
	m_width = width;
	m_height = height;
	m_bodyWidth = width - (GameWindowTexMgr::Instance().GetBorderOffset() * 2);
	m_bodyHeight = height - (GameWindowTexMgr::Instance().GetBorderOffset() * 2);
	m_winText.SetLineWidth(m_bodyWidth - 6);
}

void GameWindow::SetFont(GameFont* pgf)
{
	m_winText.SetFont(pgf);
}

void GameWindow::IgnoreSlider(const bool toggle)
{
	m_bIgnoreSlider = toggle;
}

void GameWindow::DrawFrame(DrawMgr& drawMgr)
{
	Vector2f pos = GetWorldPos();
	float x = pos.x;
	float y = pos.y;

	const Texture& body		= GameWindowTexMgr::Instance().GetWinBody();
	const Texture& corner	= GameWindowTexMgr::Instance().GetCorner();
	const Texture& topbar	= GameWindowTexMgr::Instance().GetTopBar();
	const Texture& sidebar	= GameWindowTexMgr::Instance().GetSideBar();
	const float offset		= static_cast<float>(GameWindowTexMgr::Instance().GetBorderOffset());

	// scaled window body
	drawMgr.OneTimeTint(0.0f, 0.0f, 1.0f);
	drawMgr.DrawQuad(body, Vector2f(x + offset, y + offset), DrawSpace::Screen, FlipMode::None, ScaleMode::Direct, (float)m_bodyWidth, (float)m_bodyHeight);

	// calculate each corner and bar's position then draw it
	float cx, cy, bx, by, bw, bh; // corner and bar x and ys

	// upper left corner
	cx = x;
	cy = y;
	drawMgr.DrawQuad(corner, Vector2f(cx, cy), DrawSpace::Screen, FlipMode::None, ScaleMode::Percentage, 0.5f, 0.5f);

	// upper right corner
	cx = x + m_bodyWidth;
	cy = y;
	drawMgr.DrawQuad(corner, Vector2f(cx, cy), DrawSpace::Screen, FlipMode::HFlip, ScaleMode::Percentage, 0.5f, 0.5f);

	// lower left corner
	cx = x;
	cy = y + m_bodyHeight;
	drawMgr.DrawQuad(corner, Vector2f(cx, cy), DrawSpace::Screen, FlipMode::VFlip, ScaleMode::Percentage, 0.5f, 0.5f);

	// lower right corner
	cx = x + m_bodyWidth;
	cy = y + m_bodyHeight;
	drawMgr.DrawQuad(corner, Vector2f(cx, cy), DrawSpace::Screen, FlipMode::HVFlip, ScaleMode::Percentage, 0.5f, 0.5f);

	// upper bar
	bx = x + (2 * offset);
	by = y;
	bw = m_width - (4 * offset);  // take into account the offset for each side
	bh = static_cast<float>(topbar.m_height/2);
	drawMgr.DrawQuad(topbar, Vector2f(bx, by), DrawSpace::Screen, FlipMode::None, ScaleMode::Direct, (float)bw, (float)bh);

	// lower bar
	bx = x + (2 * offset);
	by = y + m_bodyHeight;
	bw = m_width - (4 * offset);
	bh = static_cast<float>(topbar.m_height/2);
	drawMgr.DrawQuad(topbar, Vector2f(bx, by), DrawSpace::Screen, FlipMode::VFlip, ScaleMode::Direct, (float)bw, (float)bh);

	// left bar
	bx = x;
	by = y + (2 * offset);
	bw = static_cast<float>(sidebar.m_width/2);
	bh = m_height - (4 * offset);
	drawMgr.DrawQuad(sidebar, Vector2f(bx, by), DrawSpace::Screen, FlipMode::None, ScaleMode::Direct, (float)bw, (float)bh);

	// right bar
	bx = x + m_bodyWidth;
	by = y + (2 * offset);
	bw = static_cast<float>(sidebar.m_width/2);
	bh = m_height - (4 * offset);
	drawMgr.DrawQuad(sidebar, Vector2f(bx, by), DrawSpace::Screen, FlipMode::HFlip, ScaleMode::Direct, (float)bw, (float)bh);

	// decal (Optional, faces only for now)
	if (m_decal.m_texID != 0)
	{
		bx = x + m_decalPos.x;
		by = y + m_decalPos.y;
		drawMgr.DrawQuad(m_decal, Vector2f(bx, by), DrawSpace::Screen);
	}
}

GameString& GameWindow::GetGameString()
{
	return m_winText;
}

Slider& GameWindow::GetSlider()
{
	return m_slider;
}

void GameWindow::SnapToSlider()
{
	Vector2f pos = m_slider.GetLocalPos();
	SetLocalPos(pos);
}

void GameWindow::SetSlideDest(const CompassDir::Type dir)
{
	float x = 0.0f;
	float y = 0.0f;
	Vector2f pos = m_slider.GetOnScreenPos();

	switch (dir)
	{
		case CompassDir::N:
			x = pos.x;
			y = static_cast<float>(-m_height);
			break;
		case CompassDir::NE:
			x = GAME_SCREEN_WIDTH;
			y = static_cast<float>(-m_height);
			break;
		case CompassDir::E:
			x = GAME_SCREEN_WIDTH;
			y = pos.y;
			break;
		case CompassDir::SE:
			x = GAME_SCREEN_WIDTH;
			y = GAME_SCREEN_HEIGHT;
			break;
		case CompassDir::S:
			x = pos.x;
			y = GAME_SCREEN_HEIGHT;
			break;
		case CompassDir::SW:
			x = static_cast<float>(-m_width);
			y = GAME_SCREEN_HEIGHT;
			break;
		case CompassDir::W:
			x = static_cast<float>(-m_width);
			y = pos.y;
			break;
		case CompassDir::NW:
			x = static_cast<float>(-m_width);
			y = static_cast<float>(-m_height);
			break;
	}

	m_slider.SetOffScreenPos(Vector2f(x, y));
}

void GameWindow::SetDecal(const Texture& decal)
{
	m_decal = decal;
}

void GameWindow::SetDecalPos(const Position& pos)
{
	m_decalPos = pos;
}

void GameWindow::MoveDecal(const int xdir, const int ydir)
{
	m_decalPos.x += xdir;
	m_decalPos.y += ydir;
}


//////////////////////
// DialogueWindow	//
//////////////////////

DialogueWindow::DialogueWindow(const int width, const int height, const Vector2f& origin) : 
	GameWindow(width, height, origin),
	m_bWaitingForInput(false),
	m_bInputReceived(false),
	m_bWasOffScreen(false)
{
	m_letterTracker.SetRate(25);
	ResetStringPositions();
}

DialogueWindow::~DialogueWindow()
{

}

void DialogueWindow::DoUpdate(const float ticks)
{
	Vector2f pos = m_slider.GetLocalPos();
	SetLocalPos(pos);

	m_winText.SetBounds(static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f),
						m_width, m_height);

	if (AtEndOfString())
		return;

	if (m_bWasOffScreen && !(m_slider.IsOffScreen()))
	{
		ResetStringPositions();
	}
	m_bWasOffScreen = m_slider.IsOffScreen();

	m_letterTracker += ticks;
	int i = m_letterTracker.GetUnitsAsInt();
	int j = 0;
	int last = m_winText.GetLastChar();
	while (j < i)
	{
		if (m_winText.GetChar(last) != PAUSE_CHAR)
		{
			last++;
		}
		++j;
	}
	m_winText.SetLastChar(last);
	last = m_winText.GetLastChar(); // SetLastChar will make sure we are in bounds

	if ((m_winText.GetChar(last) == PAUSE_CHAR) || AtEndOfString())
		m_bWaitingForInput = true;
}

void DialogueWindow::DoDraw(DrawMgr& drawMgr)
{
	DrawFrame(drawMgr);

	// The text of the window is a child of this window
	// and will be drawn during its own DoDraw pass.

	if (m_bWaitingForInput && !AtEndOfString())
	{
		Vector2f pos = GetWorldPos();
		float x = pos.x + m_bodyWidth - 9;
		float y = pos.y + m_bodyHeight - 4;
		const Texture& blinker = GameWindowTexMgr::Instance().GetBlinker();
		drawMgr.DrawQuad(blinker, Vector2f(x, y), DrawSpace::Screen, FlipMode::None, ScaleMode::Percentage, 0.5f, 0.5f);
	}
}

void DialogueWindow::ProcessInput()
{
	if (m_bWaitingForInput &&
		InputMgr::Instance().WasPressed(GEN_A) )
	{
		if (AtEndOfString())
		{
			m_slider.SlideOffScreen();
			ResetStringPositions();
		}
		else //!AtEndOfString()
		{
			int first = m_winText.GetLastChar() + 1;
			int last = first;
			m_winText.SetLastChar(last);
			m_winText.SetFirstChar(first);
			m_bWaitingForInput = false;
		}
	}
}

void DialogueWindow::SetText(const std::string& str)
{
	m_winText.SetString(str);
	ResetStringPositions();
}

void DialogueWindow::ResetStringPositions()
{
	m_letterTracker.Reset();
	m_bWaitingForInput = false;
	m_bInputReceived = false;
	m_winText.SetLastChar(0);
	m_winText.SetFirstChar(0);
}

bool DialogueWindow::AtEndOfString()
{
	if (m_winText.GetLastChar() == m_winText.GetStringLength())
		return true;
	else
		return false;
}

///////////////////////
// AutoAdvanceWindow //
///////////////////////

AutoAdvanceWindow::AutoAdvanceWindow(const int width, const int height,const Vector2f& origin) : 
	GameWindow(width, height, origin),
	m_letterTracker(25),
	m_expireTracker(0.5f),
	m_bDoubleTextSpeed(false),
	m_bExpired(false)
{
	ResetStringPositions();
	m_slider.SnapOntoScreen();
}

AutoAdvanceWindow::~AutoAdvanceWindow()
{

}

void AutoAdvanceWindow::DoUpdate(const float ticks)
{
	Vector2f pos = m_slider.GetLocalPos();
	SetLocalPos(pos);
	//@HACK: snipping another 1 off the top since drop shadows from the line above
	// were showing up in level up windows.
//	m_winText.SetBounds(pos.x, pos.y+1, m_width, m_height-1);
	m_winText.SetBounds(static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 1.5f),
						m_width, m_height - 1);
	if (m_bExpired)
		return;

	if (!AtEndOfString())
	{
		// if the player is holding down the Confirm button, feed the letter tracker
		// a double helping of ticks
		if (m_bDoubleTextSpeed)
			m_letterTracker += ticks;

		m_letterTracker += ticks;
		int i = m_letterTracker.GetUnitsAsInt();
		int j = 0;
		int last = m_winText.GetLastChar();
		
		while (j < i)
		{
			++last;
			++j;
		}
		m_winText.SetLastChar(last);
		last = m_winText.GetLastChar(); // SetLastChar will make sure we are in bounds
	}
	else // at end of string, do delay then kill the window
	{
		// if the player is holding down the Confirm button, feed the delay tracker
		// a double helping of ticks
		//@TODO: one of the testers says that holding space to advance combat text quicker 
		// seems to be crashing his game, disabling for now to see if the bug goes away or not
//		if (m_bDoubleTextSpeed)
//			m_expireTracker += ticks;

		m_expireTracker += ticks;
		int i = m_expireTracker.GetUnitsAsInt();
		if (i > 0)
		{
			m_bExpired = true;
			RemoveChild(m_winText);
		}
	}
}

void AutoAdvanceWindow::DoDraw(DrawMgr& drawMgr)
{
//	Position pos = GetLocalPos();
	if (!m_bExpired)
		DrawFrame(drawMgr);

	// The text of the window is a child of this window
	// and will be drawn during its own DoDraw pass.
}

void AutoAdvanceWindow::ProcessInput()
{
	if (InputMgr::Instance().IsDown(GEN_A))
		m_bDoubleTextSpeed = true;
	else
		m_bDoubleTextSpeed = false;
}

void AutoAdvanceWindow::SetText(const std::string& str)
{
	RemoveChild(m_winText);
	AddChild(m_winText);
	m_winText.SetString(str);
	ResetStringPositions();
	m_bExpired = false;
}

void AutoAdvanceWindow::ResetStringPositions()
{
	m_letterTracker.Reset();
	m_expireTracker.Reset();
	m_winText.SetLastChar(0);
	m_winText.SetFirstChar(0);
	m_winText.ResetScrolling();
}

bool AutoAdvanceWindow::AtEndOfString() const
{
	if (m_winText.GetLastChar() == m_winText.GetStringLength())
		return true;
	else
		return false;
}

bool AutoAdvanceWindow::IsExpired() const
{
	return m_bExpired;
}


//////////////////////
// GameWindowTexMgr	//
//////////////////////

GameWindowTexMgr::GameWindowTexMgr() : 
	m_spellset("sfspells.bmp"),
	m_itemset("sfitems.bmp")
{

}

void GameWindowTexMgr::Init()
{
	Image buffer;
	const std::string imagepath = FilePathMgr::Instance().GetImagePath();

	buffer.LoadBMP(imagepath + "gradient.bmp", true);
	m_body.LoadImage(buffer);

	buffer.LoadBMP(imagepath + "corner.bmp");
	m_corner.LoadImage(buffer);

	buffer.LoadBMP(imagepath + "vbar.bmp");
	m_sidebar.LoadImage(buffer);

	buffer.LoadBMP(imagepath + "hbar.bmp");
	m_topbar.LoadImage(buffer);

	buffer.LoadBMP(imagepath + "triangle.bmp");
	m_blinker.LoadImage(buffer);

	buffer.LoadBMP(imagepath + "redbracket.bmp");
	m_bracket.LoadImage(buffer);
}

const Texture& GameWindowTexMgr::GetWinBody() const
{
	return m_body;
}

const Texture& GameWindowTexMgr::GetCorner() const
{
	return m_corner;
}	

const Texture& GameWindowTexMgr::GetTopBar() const
{
	return m_topbar;
}

const Texture& GameWindowTexMgr::GetSideBar() const
{
	return m_sidebar;
}

const Texture& GameWindowTexMgr::GetBlinker() const
{
	return m_blinker;
}

const Texture& GameWindowTexMgr::GetBracket() const
{
	return m_bracket;
}

const Texture& GameWindowTexMgr::GetSpellIcon(SpellName::Type icon) const
{
	return m_spellset.GetSpellIcon(icon);
}

const Texture& GameWindowTexMgr::GetItemIcon(ItemGfxID::Type icon) const
{
	return m_itemset.GetItemIcon(icon);
}

int GameWindowTexMgr::GetBorderOffset() const
{
	return m_corner.m_dataWidth / 4;
}