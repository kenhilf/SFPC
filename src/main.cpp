#include "gameengine.h"

// here we handle windows messages
LRESULT CALLBACK WndProc(HWND hWnd,			// handle for this window
						 UINT uMsg,			// message for this window
						 WPARAM wParam,		// additional msg info
						 LPARAM lParam)		// additional msg info
{
	switch (uMsg)
	{
		// check to see if the window is still active.  if minimized, active = false, else true. 
		case WM_ACTIVATE:
		{
			if (!HIWORD(wParam))
			{
				GameEngine::Instance().SetActive(true);
			}
			else
			{
				GameEngine::Instance().SetActive(false);
			}

			return 0;
		}

		// restrict input when the game window is not the active window
		case WM_SETFOCUS:
		{
			GameEngine::Instance().SetActive(true);
			return 0;
		}
		case WM_KILLFOCUS:
		{
			GameEngine::Instance().SetActive(false);
			return 0;
		}

		// here we're looking for if the system is trying to suspend power to the monitor
		// or turn on the screen saver.  returning 0 suppresses that.
		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		}

		// if the window has been closed, a quit message is sent that the main loop will 
		// intercept.  done will be set to true, winmain will stop, and the program will close.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// if the window has been resized, call ReSizeGLScene to make the adjustments
		case WM_SIZE:
		{
			bluGL::Instance().ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
			return 0;
		}
	}

	// pass all unhandled messages to DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// in winmain we create the window, deal with messages, and watch for human interaction
int WINAPI WinMain(HINSTANCE hInstance,			// instance
				   HINSTANCE hPrevInstance,		// previous instance
				   LPSTR lpCmdLine,				// command line parameters
				   int nCmdShow)				// window show state
{
	GameEngine::Instance().Initialize();

	// windows message structure
	MSG msg;
	while (true)
	{
		// check to see if we have any messages waiting
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// if the current message is a WM_QUIT from PostQuitMessage(0)
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				// Translate and dispatch the message for WndProc() or Windows to deal with.
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		// GameLoop returns true until Esc is pressed to exit, which returns false
		if(GameEngine::Instance().GameLoop() == false)
			break;
	}

	// Calls the singleton of the GameEngine to shut down everything game related
	GameEngine::Instance().Shutdown();

	return static_cast<int>(msg.wParam);
}
