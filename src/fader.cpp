#include "fader.h"

Fader::Fader(const float faderate) :
	m_fadeTracker(faderate),
	m_fadePct(1.0f),
//	m_bFadeIn(false)
	m_state(FadeState::Idle)
{

}

Fader::~Fader()
{

}

void Fader::FadeIn()
{
//	m_bFadeIn = true;
	m_fadePct = 0.0f;
	m_state = FadeState::FadingIn;
	m_fadeTracker.Reset();
}

void Fader::FadeOut()
{
//	m_bFadeIn = false;
	m_fadePct = 1.0f;
	m_state = FadeState::FadingOut;
	m_fadeTracker.Reset();
}

void Fader::Clear()
{
	m_state = FadeState::Idle;
	m_fadePct = 1.0f;
}

bool Fader::DoneFadingIn() const
{
//	if ((m_bFadeIn) && (m_fadePct >= 1.0f))
//	if ((m_state == FadeState::FadingIn) && (m_fadePct >= 1.0f))
	if (m_fadePct >= 1.0f)
		return true;
	else
		return false;
}

bool Fader::DoneFadingOut() const
{
//	if ((!m_bFadeIn) && (m_fadePct <= 0.0f))
//	if ((m_state == FadeState::FadingOut) && (m_fadePct <= 0.0f))
	if (m_fadePct <= 0.0f)
		return true;
	else
		return false;
}

bool Fader::IsIdle() const
{
	if (m_state == FadeState::Idle)
		return true;
	else
		return false;
}

void Fader::SetRate(const float faderate)
{
	m_fadeTracker.SetRate(faderate);
}

void Fader::DoUpdate(const float ticks)
{
	if (m_state == FadeState::Idle)
		return;
		
	m_fadeTracker += ticks;
	const int change = m_fadeTracker.GetUnitsAsInt();
	if (change > 0)
	{
//		if (m_bFadeIn)
		if (m_state == FadeState::FadingIn)
		{
			m_fadePct += (static_cast<float>(change) * 0.01f);
			if (m_fadePct > 1.0f)
			{
				m_fadePct = 1.0f;
				m_state = FadeState::Idle;
			}
		}
//		else // fading out
		else if (m_state == FadeState::FadingOut)
		{
			m_fadePct -= (static_cast<float>(change) * 0.01f);
			if (m_fadePct < 0.0f)
			{
				m_fadePct = 0.0f; 
				m_state = FadeState::Idle;
			}
		}
		else // (m_state == FadeState::Idle)
		{
			// do nothing
		}
	}
}

void Fader::DoDraw(DrawMgr& drawMgr)
{
	drawMgr.GlobalFade(m_fadePct);
}