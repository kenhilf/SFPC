#ifndef _aniindex_h_
#define _aniindex_h_

#include "gametimer.h"

namespace AniType 
{
	// ex: 3 frames
	// Loop: 1, 2, 3, 1, 2, 3, 1, 2, 3...
	// PingPong: 1, 2, 3, 2, 1, 2, 3, 2, 1...
	// Sequence: 1, 2, 3, 3, 3, 3, 3, 3, 3
	enum Type { Loop, PingPong, Sequence, NumTypes };
}

class AnimationIndexer
{
public:
	AnimationIndexer();

	void Init(const int numFrames, const int msPerFrame, const AniType::Type aniType);
	void Reset() { m_frameTimer.Reset(); m_currFrame = 0; m_bActive = true;}
	void Update(const float ticks);

	int GetCurrFrame() const { return m_currFrame; }
	int GetNumFrames() const { return m_numFrames; }
	void SetFrame(const int frame) { m_currFrame = (frame % m_numFrames); m_frameTimer.Reset(); }

private:
	int m_numFrames;
	int m_currFrame;
	AniType::Type m_aniType;
	GameTimer::Tracker m_frameTimer;
	bool m_bActive;
};

#endif //_aniindex_h_


