#include "blugl.h"

bluGL::bluGL() :
	m_hRC(NULL),
	m_hDC(NULL),
	m_hWnd(NULL),
	m_bActive(true),
	m_bFullscreen(false),
	m_bInitialized(false),
	m_pWinTitle("bludragn's OpenGL Framework"),
	m_winWidth(640),
	m_winHeight(480),
	m_bpp(32)
{
	//@TODO: This shouldn't be here - nothing to do with OpenGL
	srand((unsigned int)time(0));
}

void bluGL::LoadTextures()
{
	// non-game related system textures would go here
}

GLvoid bluGL::ReSizeGLScene(GLsizei width, GLsizei height)
{
	if (height == 0)
	{
		height = 1;
	}

	m_winWidth = width;
	m_winHeight = height;

	// Reset the current viewport
	glViewport(0, 0, width, height);

	// Select and reset the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set up 2D orthographic projection with 0, 0 as the top left corner of the screen
	gluOrtho2D(0, static_cast<GLdouble>(GAME_SCREEN_WIDTH), static_cast<GLdouble>(GAME_SCREEN_HEIGHT), 0);
	
	// Select and reset the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool bluGL::Initialize()
{
	if (!m_bInitialized)
	{
		// Enable VSync
		SetVSyncEnabled(false);

		// This currently does nothing, might eventually make it load standard non-game textures
		LoadTextures();

		// enable texture mapping
		glEnable(GL_TEXTURE_2D);

		// enable smooth shading
		glShadeModel(GL_SMOOTH);

		// clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		// depth buffer setup, enable depth testing and type of depth testing
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDepthFunc(GL_LEQUAL);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// If alpha value is 0, renderer will discard it
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);

		// enable nicest perspective calculations
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		return true;
	}

	return false;
}

void bluGL::PreDrawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void bluGL::PostDrawGLScene()
{
	SwapBuffers(m_hDC);
}

GLvoid bluGL::KillGLWindow(GLvoid)
{
	if (m_bFullscreen)
	{
		// Switch back to the desktop, restore the mouse pointer
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(true);
	}

	// if there is still a rendering context to release
	if(m_hRC)
	{
		// attempt to detatch the rendering context from the device context
		if (!wglMakeCurrent(NULL, NULL))
		{
			MessageBox(NULL, "Release of DC and RC failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		
		// attempt to delete the rendering context
		if (!wglDeleteContext(m_hRC))
		{
			MessageBox(NULL, "Release Rendering Context failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		// rendering context released, set handle to null
		m_hRC = NULL;
	}

	// Wash, rinse, repeat for Device Context
	if (m_hDC && !ReleaseDC(m_hWnd, m_hDC))
	{
		MessageBox(NULL, "Release Device Context failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		m_hWnd = NULL;
	}

	// Now to destroy the window
	if (m_hWnd && !DestroyWindow(m_hWnd))
	{
		MessageBox(NULL, "Could not release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		m_hWnd = NULL;
	}

	// Lastly the windows class is unregistered
	if (!UnregisterClass("OpenGL", m_hInstance))
	{
		MessageBox(NULL, "Could not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		m_hInstance = NULL;
	}

}

bool bluGL::CreateGLWindow(char* title, const int width, const int height, const int bits, const bool bFullScreen)
{
	m_pWinTitle = title;
	m_bpp = bits;
	m_bFullscreen = bFullScreen;

	InitWindowSize(width, height, m_bFullscreen); // Inits m_winWidth and m_winHeight

	return CreateGLWindow();
}

bool bluGL::CreateGLWindow()
{
	// holds the results of a request to windows for a pixel format
	GLuint		PixelFormat;

	// window class structure holding window attributes
	WNDCLASS	wc;

	// extended and normal window style variables
	DWORD		dwExStyle;
	DWORD		dwStyle;

	// grab an instance of the window and declare the window class
	m_hInstance = GetModuleHandle(NULL);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// Redraw on resize/move and get a private 
														//   DC not shared across applications
	wc.lpfnWndProc = (WNDPROC) WndProc;					// WndProc handles messages
	wc.cbClsExtra = 0;									// No extra window data
	wc.cbWndExtra = 0;									// Still no extra window data
	wc.hInstance = m_hInstance;							// Set the instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);				// Load the default icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load the arrow pointer
	wc.hbrBackground = NULL;							// No background necessary
	wc.lpszMenuName = NULL;								// No menu necessary
	wc.lpszClassName = "OpenGL";						// Set the class name

	// Register the class
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, "Failed to register the window class.", "ERROR", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	// If running in fullscreen mode, shift to fullscreen mode
	if (m_bFullscreen)
	{
		// Set up device mode.  Wipe it, then set width, height, and bpp
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = m_winWidth;
		dmScreenSettings.dmPelsHeight = m_winHeight;
		dmScreenSettings.dmBitsPerPel = m_bpp;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Attempt to set the screen to the selected mode.  CDS_FULLSCREEN is to hide the taskbar
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If we're here, we were not successful and have two options: quit or try windowed mode
			if (MessageBox(NULL, "The requested fullscreen mode is not supported by \nyour video card.  Use windowed mode instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				// switch gears to windowed mode
				m_bFullscreen = false;
			}
			else
			{
				// user has opted to quit.  exit and return false.
				MessageBox(NULL, "Program will now close.", "Error", MB_OK | MB_ICONSTOP);
				return false;
			}
		}
	}

	// check again for fullscreen as the code above may have failed and the user may have switched to windowed mode
	if (m_bFullscreen)
	{
		// Set extended style to WS_EX_APPWINDOW, window style WS_POPUP gives us no borders, 
		// and disable the mouse pointer
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(false);
	}

	// we're in windowed mode.  WS_EX_WINDOWEDGE gives the window a 3d look.
	// WS_OVERLAPPEDWINDOW adds a title bar, sizing border, window menu, and min/max buttons.
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
	}

	// AdjustWindowRect expands the window so that the body of the window is the size we requested
	// instead of having the edges get overlapped by the borders.  If we're fullscreen it does nothing.
	RECT windowRect = { 0, 0, m_winWidth, m_winHeight };
	AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);

	int winX = CW_USEDEFAULT;
	int winY = CW_USEDEFAULT;
	int winWidth = windowRect.right - windowRect.left;
	int winHeight = windowRect.bottom - windowRect.top;

	bool bCenterWindow = !m_bFullscreen;
	if (bCenterWindow)
	{
		winX = (::GetSystemMetrics(SM_CXSCREEN) - winWidth) / 2;
		winY = (::GetSystemMetrics(SM_CYSCREEN) - winHeight) / 2;
	}

	// finally we attempt to create the window we have configured
	if (!(m_hWnd = CreateWindowEx(	dwExStyle,							// Extended Window Style
									"OpenGL",							// Class name
									m_pWinTitle,						// Window title
									WS_CLIPSIBLINGS | WS_CLIPCHILDREN |	// required window styles for OpenGL
									dwStyle,							// OR'ed on top of our existing dwStyle
									winX, winY,							// place the window at 8, 8 on the screen
									winWidth,							// calculate adjusted window width
									winHeight,							// calculate adjusted window height
									NULL,								// no parent window
									NULL,								// no menu
									m_hInstance,						// instance
									NULL)))								// pass nothing to WM_CREATE
	// if we succeeded, we have a window handle in hWnd.  If not, quit.
	{
		KillGLWindow();
		MessageBox(NULL, "Window creation failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// Set up a pixel format.  We want a format that supports OpenGL, double buffering, RGBA channels, our chosen
	// bit depth, and a 16bit zbuffer.
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),		// size of this pixel format descriptor
		1,									// version number
		PFD_DRAW_TO_WINDOW |				// format must support window, 
		PFD_SUPPORT_OPENGL |				// as well as opengl
		PFD_DOUBLEBUFFER,					// and double buffering,
		PFD_TYPE_RGBA,						// in RGBA format
		m_bpp,								// in our requested color depth.
		0, 0, 0, 0, 0, 0,					// color bits ignored
		0,									// no alpha buffer
		0,									// shift bit ignored
		0,									// no accumulation buffer
		0, 0, 0, 0,							// accumulation bits ignored
		16,									// 16bit zbuffer
		0,									// no stencil buffer
		0,									// no auxiliary buffer
		PFD_MAIN_PLANE,						// main drawing layer
		0,									// reserved
		0, 0, 0								// layer masks ignored
	};

	// Now we attempt to get an OpenGL device context.
	if (!(m_hDC = GetDC(m_hWnd)))
	{
		KillGLWindow();
		MessageBox(NULL, "Can't create a GL Device Context.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// Ask windows for a pixel format that fits our requirements.
	if (!(PixelFormat = ChoosePixelFormat(m_hDC, &pfd)))
	{
		KillGLWindow();
		MessageBox(NULL, "Can't find a suitable pixel format.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// If we got a pixel format back from windows, set it.
	if (!SetPixelFormat(m_hDC, PixelFormat, &pfd))
	{
		KillGLWindow();
		MessageBox(NULL, "Can't set the pixel format.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// attempt to obtain a rendering context
	if (!(m_hRC = wglCreateContext(m_hDC)))
	{
		KillGLWindow();
		MessageBox(NULL, "Can't create a GL Rendering Context.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// make the rendering context active
	if (!wglMakeCurrent(m_hDC, m_hRC))
	{
		KillGLWindow();
		MessageBox(NULL, "Can't activate the GL Rendering Context.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// If everything has been set up and created successfully, now we show the window, set it to the foreground,
	// give it focus, then call ResizeGLScene to set up the perspective OpenGL screen.
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);					// sets the keyboard focus to this window
	ReSizeGLScene(m_winWidth, m_winHeight);

	// finally we call InitGL to set up our own material: lighting, textures, etc.
	if (!Initialize())
	{
		KillGLWindow();
		MessageBox(NULL, "Initalization failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// Making it this far implies success, so return true
	return true;
}

bool bluGL::ToggleFullscreen()
{
	KillGLWindow();
	m_bFullscreen = !m_bFullscreen;

	return CreateGLWindow();
}

void bluGL::SetVSyncEnabled(const bool bEnabled)
{
	// If the Windows OpenGL extension for setting vsync exists, turn it on.
	// What we're actually doing is setting the number of vsyncs SwapBuffers
	// should wait before actually swapping the buffers.

	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

	const char *extensions = (const char*)glGetString(GL_EXTENSIONS);

	if (strstr(extensions, "WGL_EXT_swap_control") == 0)
	{
		return; // Error: WGL_EXT_swap_control extension not supported on your computer
	}
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (wglSwapIntervalEXT)
		{
			wglSwapIntervalEXT(bEnabled? 1 : 0);
		}
	}
}

namespace WindowSizeMethod
{
	enum Type
	{
		Input,						// Use input width, height values
		LargestGameScreenMultiple,	// Largest possible window that is a multiple of game screen dimensions (pixel-perfect)
		FillScreen					// Largest possible window, maintains aspect ratio (not pixel-perfect)
	};
}

void bluGL::InitWindowSize(int width, int height, bool fullScreen)
{
	//WindowSizeMethod::Type windowSizeMethod = WindowSizeMethod::Input;
	WindowSizeMethod::Type windowSizeMethod = WindowSizeMethod::LargestGameScreenMultiple;
	//WindowSizeMethod::Type windowSizeMethod = WindowSizeMethod::FillScreen;

	if (fullScreen)
	{
		windowSizeMethod = WindowSizeMethod::Input;
	}

	if (windowSizeMethod == WindowSizeMethod::Input)
	{
		m_winWidth = width;
		m_winHeight = height;
	}
	else if (windowSizeMethod == WindowSizeMethod::LargestGameScreenMultiple)
	{
		const float screenWidth = (float)::GetSystemMetrics(SM_CXSCREEN);
		const float screenHeight = (float)::GetSystemMetrics(SM_CYSCREEN);

		int scale = 1;
		if (screenWidth >= screenHeight)
		{
			scale = static_cast<int>(screenHeight / GAME_SCREEN_HEIGHT);
		}
		else
		{
			scale = static_cast<int>(screenWidth / GAME_SCREEN_WIDTH);
		}

		m_winWidth = width * scale;
		m_winHeight = height * scale;
	}
	else if (windowSizeMethod == WindowSizeMethod::FillScreen)
	{
		const int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
		const int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		const int cushion = 60; // Take window borders and taskbar into account

		if (screenWidth >= screenHeight)
		{
			m_winHeight = screenHeight - cushion;
			m_winWidth = static_cast<int>(m_winHeight * (static_cast<float>(width) / height));
		}
		else
		{
			m_winWidth = screenWidth - cushion;
			m_winHeight = static_cast<int>(m_winWidth * (static_cast<float>(height) / width));
		}
	}
}
