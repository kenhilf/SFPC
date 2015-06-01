#ifndef _blugl_h_
#define _blugl_h_

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "core.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class bluGL
{
public:
	static bluGL& Instance() { static bluGL instance; return instance; }
	void LoadTextures();
	GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
	bool Initialize();
	void PreDrawGLScene();
	void PostDrawGLScene();
	GLvoid KillGLWindow(GLvoid);
	bool CreateGLWindow();
	bool CreateGLWindow(char* title, const int width, const int height, const int bits, const bool bFullScreen);
	void ResetView();
	bool ToggleFullscreen();
	void SetVSyncEnabled(const bool bEnabled);

	int GetWindowWidth() const { return m_winWidth; }
	int GetWindowHeight() const { return m_winHeight; }

private:
	// hiding constructor for singleton pattern
	bluGL();

	// disable copy constructor and =operator
	bluGL(const bluGL& rhs);
	bluGL& operator=(const bluGL& rhs);

	HGLRC m_hRC;				// handle to Rendering Context
	HDC	m_hDC;					// handle to Device Context
	HWND m_hWnd;				// handle to the window
	HINSTANCE m_hInstance;		// handle to the instance of the app

	bool m_bActive;
	bool m_bFullscreen;
	bool m_bInitialized;

	char* m_pWinTitle;
	int m_winWidth;
	int m_winHeight;
	int m_bpp;
};

#endif //_blugl_h_