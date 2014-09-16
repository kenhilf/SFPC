#ifndef _animation_h_
#define _animation_h_

#include "texture.h"
#include "aniindex.h"
#include <vector>
class Tileset;

class Animation
{
public:
	Animation();
	Animation(Tileset* ts);

	void DoUpdate(const float ticks);
	void SetTileset(Tileset* ts);
	void AddFrame(const int frame);
	void Reset();
	const Texture& GetActiveFrame() const;
	const Texture& GetFrame(const int index) const;
	AnimationIndexer& GetAnimationIndexer() { return m_aniIndex; }

private:
	typedef std::vector<int> TexIndexList;
	TexIndexList m_frames;

	Tileset* m_pTileset;

	AnimationIndexer m_aniIndex;
};

#endif //_animation_h_