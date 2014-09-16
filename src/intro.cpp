#include "intro.h"
#include "texture.h"
#include "inputmgr.h"
#include "gameobjectmgr.h"
#include "filepathmgr.h"
#include "audiomgr.h"
#include "commandmgr.h"

GameIntro::GameIntro() :
	GameObject(LayerSpace::Window),
	m_fadeTracker(25),
	m_fadePct(-0.5f),
	m_bFadeIn(true),
	m_bExpired(false),
	m_bFirstSpace(false),
	m_bShowInstructions(false)
{
	m_openingText.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_openingText.SetString("  Press SPACE\n \n \n \n \n \n Ken Hilf 2010");
	m_openingText.SetOffset(104, 150);
	AddChild(m_openingText);

	Image buffer;
	std::string imagepath = FilePathMgr::Instance().GetImagePath();
	buffer.LoadBMP(imagepath + "logo.bmp");
	m_logo.LoadImage(buffer);
	buffer.LoadBMP(imagepath + "black.bmp");
	m_black.LoadImage(buffer);
	buffer.LoadBMP(imagepath + "keyboard.bmp");
	m_keyboard.LoadImage(buffer);

	AudioMgr::PlaySong("headquarters.ogg");
}

GameIntro::~GameIntro()
{

}

void GameIntro::DoUpdate(const float ticks)
{
	m_fadeTracker += ticks;

	const int change = m_fadeTracker.GetUnitsAsInt();
	if (change > 0)
	{
		if (m_bFadeIn)
		{
			m_fadePct += (static_cast<float>(change) * 0.01f);
		}
		else // fading out
		{
			m_fadePct -= (static_cast<float>(change) * 0.01f);
		}

		if (m_fadePct > 1.0f)
		{
			m_fadePct = 1.0f; 
		}
		if ((m_fadePct < 0.0f) && (!m_bFadeIn))
		{
			m_bExpired = true;
		}
	}
}

void GameIntro::DoDraw(DrawMgr& drawMgr)
{
	drawMgr.GlobalFade(m_fadePct);
	drawMgr.DrawQuad(m_black, Vector2f(0.0f, 0.0f), DrawSpace::Screen, FlipMode::None, ScaleMode::Direct, static_cast<float>(GAME_SCREEN_WIDTH), static_cast<float>(GAME_SCREEN_HEIGHT));
	if (!m_bShowInstructions)
		drawMgr.DrawQuad(m_logo, Vector2f(32.0f, 48.0f), DrawSpace::Screen);
	else
		drawMgr.DrawQuad(m_keyboard, Vector2f(40.0f, 128.0f), DrawSpace::Screen);

	if (m_bExpired)
	{
		drawMgr.ResetRGBA();
	}
}

void GameIntro::ProcessInput()
{
	if (m_bExpired)
	{
		if (GameObjectMgr::Instance().CheckCurrentFocus(this))
		{
			GameObjectMgr::Instance().PopFocusObject();
			GameObjectMgr::Instance().RemoveObject(*this);
		}
	}

	if (InputMgr::Instance().WasPressed(GEN_A) && m_bFirstSpace &&
		m_bFadeIn && m_bShowInstructions) 
	{
		m_bFadeIn = false;

		CommandMgr::Instance().AddCommandManually(new GCWait(4000));
		CommandMgr::Instance().AddCommandManually(new GCPlayMusic("tension.mp3"));
		CommandMgr::Instance().AddCommandManually(new GCSetFlag("IntroDone"));
	}

	if (InputMgr::Instance().WasPressed(GEN_A) && m_bFirstSpace &&
		m_bFadeIn && !m_bShowInstructions)
	{
		AudioMgr::PlaySoundEffect("chime.ogg");
		m_fadePct = 0.0f;

		m_openingText.SetString("\n \n            - How to Play -\n \n \n*5Arrow Keys - Movement, Make Selections\n \n          *2Spacebar - Confirm\n \n              *3C - Cancel\n \n               *4X - Menu\n \n            *0Esc - Quit Game\n \n \n \n \n \n \n \n \n \n \n \n \n        Press SPACE to Continue");
		m_openingText.SetOffset(8, 4);

		m_bShowInstructions = true;
	}

	if (InputMgr::Instance().WasPressed(GEN_A) && !m_bFirstSpace)
	{
		m_bFirstSpace = true;
		m_fadePct = 1.0f;
		m_openingText.SetString(" > New Game\n \n*8   Load Game\n \n   Options");
	}
}
