/* WinMain.cpp
Author: Jeff Kiah
Started: 5/16/12
*/

#include "Win32.h"
#include <crtdbg.h>
#include <cstdlib>
#include <memory>
#include "Application/Settings.h"
#include "Application/Application.h"
#include "Application/Timer.h"

using std::unique_ptr;

Win32 win32(L"Icarus", L"ICARUSCLASS");

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	srand(GetTickCount());

	Settings settings;
	win32.init(hInstance, &settings);
	if (!win32.isOnlyInstance()) { return 0; } // allow only a single instance
	if (!win32.checkCapabilities()) { return 0; } // make sure this computer can run the game

	#ifdef _DEBUG
		// Set up debug heap checking	
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif
	#ifdef DEBUG_CONSOLE
		// Open a console and redirect stdout to it
		win32.initConsole();
	#endif
	
	if (!win32.getSystemInformation()) { return 0; } // some requirement not met

	// init Timer
	if (!Timer::initHighPerfTimer()) {
		win32.showErrorBox(L"Failed to initialize high-performance counter");
		return 0;
	}

	// init window and input devices
	if (!win32.initWindow()) { return 0; }
	if (!win32.initInputDevices()) { return 0; }

	// initialize the application
	ApplicationFactory appFactory((const Platform *)(&win32), &settings);
	ApplicationUniquePtr app(appFactory.createIcarus());
	if (!app) {
		win32.showErrorBox(L"Failed to initialize application");
		return 0;
	}

	MSG msg;
	SecureZeroMemory(&msg, sizeof(msg));

	for (;;) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) { break; }

			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
		} else if (win32.isActive()) {
			if (win32.isExiting()) { PostMessage(win32.hWnd, WM_CLOSE, 0, 0); }

			app->processFrame();

		} else {
			WaitMessage();	// avoid 100% CPU when inactive
		}
	}

	// Check for corruption in the heap, generate debug report	
	_ASSERTE(_CrtCheckMemory());

	return (int)msg.wParam;
}