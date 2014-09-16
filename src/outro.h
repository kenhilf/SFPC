#ifndef _outro_h_
#define _outro_h_

#include "intro.h"

class GameOutro : public GameIntro
{
public:
	GameOutro();
	virtual ~GameOutro();
	virtual void ProcessInput();
};

#endif // _outro_h_