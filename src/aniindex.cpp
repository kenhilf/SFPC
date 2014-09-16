#include "aniindex.h"
#include <assert.h>

AnimationIndexer::AnimationIndexer() :
	m_numFrames(1),
	m_currFrame(0),
	m_aniType(AniType::Loop),
	m_frameTimer(250),
	m_bActive(false)
{

}

void AnimationIndexer::Init(const int numFrames, const int msPerFrame, const AniType::Type aniType)
{
	m_numFrames = numFrames;
	m_currFrame = 0;
	m_aniType = aniType;
	m_bActive = true;

	const float framesPerMS = 1.0f / static_cast<float>(msPerFrame);
	const float framesPerSec = framesPerMS * 1000.0f;
	m_frameTimer.SetRate(framesPerSec);
}

void AnimationIndexer::Update(const float ticks)
{
	if (!m_bActive)
		return;

	m_frameTimer += ticks;
	
	if (m_aniType == AniType::Loop)
	{
		int elapsedFrames = m_frameTimer.GetUnitsAsInt();
		m_currFrame = (m_currFrame + elapsedFrames) % m_numFrames;
	}
	else if (m_aniType == AniType::PingPong)
	{
		assert(false && "Not yet implemented - Ping Pong animation type");
	}
	else if (m_aniType == AniType::Sequence)
	{
		if (m_currFrame == (m_numFrames-1))
			m_bActive = false;
		int elapsedFrames = m_frameTimer.GetUnitsAsInt();
		m_currFrame = (m_currFrame + elapsedFrames) % m_numFrames;
	}
}

