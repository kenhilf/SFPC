#ifndef _fader_h_
#define _fader_h_

#include "gameobject.h"
#include "gametimer.h"

namespace FadeState
{
	enum Type { FadingIn, FadingOut, Idle, NumTypes };
}

class Fader : public GameObject
{
public:
	Fader(const float faderate = 100.0f);
	virtual ~Fader();
	void FadeIn();
	void FadeOut();
	void Clear();
	bool DoneFadingIn() const;
	bool DoneFadingOut() const;
	bool IsIdle() const;
	void SetRate(const float faderate);

	virtual void DoUpdate(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);

private:
	GameTimer::Tracker m_fadeTracker;
	float m_fadePct;
//	bool m_bFadeIn;
	FadeState::Type m_state;
};

#endif //_fader_h_