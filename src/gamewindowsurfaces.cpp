#include "gamewindowsurfaces.h"
#include "dxwrap.h"

void GameWindowSurfaces::Init()
{
	// load each surface and set transparency color where appropriate

	// main window
	m_windowBody.LoadBitmap("blue.bmp");

	// corners
	m_cornerUL.LoadBitmap("ulcor.bmp");
	m_cornerUL.SetTransparency(true, _RGB(0, 255, 0, 255));
	m_cornerUR.LoadBitmap("urcor.bmp");
	m_cornerUR.SetTransparency(true, _RGB(0, 255, 0, 255));
	m_cornerLL.LoadBitmap("llcor.bmp");
	m_cornerLL.SetTransparency(true, _RGB(0, 255, 0, 255));
	m_cornerLR.LoadBitmap("lrcor.bmp");
	m_cornerLR.SetTransparency(true, _RGB(0, 255, 0, 255));

	// bars
	m_barTop.LoadBitmap("topbar.bmp");
	m_barBottom.LoadBitmap("btmbar.bmp");
	m_barLeft.LoadBitmap("lftbar.bmp");
	m_barRight.LoadBitmap("rgtbar.bmp");

	m_bInitialized = true;
}