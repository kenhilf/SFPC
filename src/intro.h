#ifndef _intro_h_
#define _intro_h_

#include "gameobject.h"
#include "gamestring.h"
#include "gametimer.h"

class GameIntro : public GameObject
{
public:
	GameIntro();
	virtual ~GameIntro();
	virtual void DoUpdate(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);
	virtual void ProcessInput();

protected:
	GameString m_openingText;
	Texture m_logo;
	Texture m_black;
	Texture m_keyboard;
	GameTimer::Tracker m_fadeTracker;
	float m_fadePct;
	bool m_bFadeIn;
	bool m_bExpired;
	bool m_bFirstSpace;
	bool m_bShowInstructions;
};

#endif // _intro_h_