#include "animation.h"
#include <assert.h>
#include "tileset.h"

Animation::Animation() :
	m_pTileset(NULL)
{

}

Animation::Animation(Tileset* ts) :
	m_pTileset(ts)
{

}

void Animation::DoUpdate(const float ticks)
{
	m_aniIndex.Update(ticks);
}

void Animation::SetTileset(Tileset* ts)
{
	m_pTileset = ts;
}

void Animation::AddFrame(const int frame)
{
	m_frames.push_back(frame);
}

void Animation::Reset()
{
	m_aniIndex.Reset();
}

const Texture& Animation::GetActiveFrame() const
{
	const int i = m_aniIndex.GetCurrFrame();
	return m_pTileset->GetTile(m_frames[i]);
}

const Texture& Animation::GetFrame(const int index) const
{
	assert(index < m_aniIndex.GetNumFrames());
	return m_pTileset->GetTile(m_frames[index]);
}