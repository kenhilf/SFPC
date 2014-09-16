#ifndef _fpscounter_h_
#define _fpscounter_h_

#include "gameobject.h"
#include "gamestring.h"
#include <sstream>

class FPSCounter : public GameObject
{
public:
	FPSCounter();
	~FPSCounter();

	virtual void DoUpdate(const float ticks);

private:
	float m_elapsedSeconds;
	float m_elapsedFrames;
	float m_fps;
	float m_avgfps;

	GameString m_output;
	std::stringstream m_strbuff;
};

#endif //_fpscounter_h_