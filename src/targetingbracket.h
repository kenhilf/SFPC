#ifndef _targetingbracket_h_
#define _targetingbracket_h_

#include "gameobject.h"
#include "tileset.h"

class TargetingBracket : public GameObject
{
public:
	TargetingBracket();
	virtual ~TargetingBracket();

	virtual void DoUpdate(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);
	virtual void ProcessInput();

private:
	Tileset m_bracket;
};

#endif //_targetingbracket_h_