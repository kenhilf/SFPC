#ifndef _gamewindowsurfaces_h_
#define _gamewindowsurfaces_h_

#include "surface.h"
#include <assert.h>

class GameWindowSurfaces
{
public:
	GameWindowSurfaces() { m_bInitialized = false; }

	// loads surfaces into the member vars, cannot be done during construction
	void Init();

	// surface access functions
	Surface& GetWindowBody() { assert(m_bInitialized); return m_windowBody; }

	Surface& GetCornerUL()	 { assert(m_bInitialized); return m_cornerUL; }
	Surface& GetCornerUR()   { assert(m_bInitialized); return m_cornerUR; }
	Surface& GetCornerLL()   { assert(m_bInitialized); return m_cornerLL; }
	Surface& GetCornerLR()   { assert(m_bInitialized); return m_cornerLR; }

	Surface& GetBarTop()	{ assert(m_bInitialized); return m_barTop; }
	Surface& GetBarBottom()	{ assert(m_bInitialized); return m_barBottom; }
	Surface& GetBarLeft()	{ assert(m_bInitialized); return m_barLeft; }
	Surface& GetBarRight()	{ assert(m_bInitialized); return m_barRight; }


private:
	bool m_bInitialized;

	// the window body
	Surface m_windowBody;

	// the four corner surfaces
	Surface m_cornerUL;
	Surface m_cornerUR;
	Surface m_cornerLL;
	Surface m_cornerLR;

	// the four edge bars
	Surface m_barTop;
	Surface m_barBottom;
	Surface m_barLeft;
	Surface m_barRight;
};


#endif //_gamewindowsurfaces_h_


