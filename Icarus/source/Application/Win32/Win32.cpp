/* Win32.cpp
Author:		Jeff Kiah
Orig.Date:	5/16/12
*/

#include "Win32.h"
#include "hidusage.h"
#include <shlobj.h>
#include <sstream>
#include <xnamath.h>
#include "Utility/Debug.h"
#include "Application/Settings.h"

// Definitions

#define CONSOLE_X			1700	// default location of the debug console window
#define CONSOLE_Y			20
#define CONSOLE_MAX_LINES	2000

// Variables
extern Win32 win32;

// Functions

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return win32.wndProc(hwnd, msg, wParam, lParam);
}

bool Win32::isOnlyInstance()
{
	// use a random GUID for the named mutex of this app
	hMutex = CreateMutex(NULL, TRUE, L"4045bd1d-76d0-41af-8d3b-53f654d33165");
	if (GetLastError() != ERROR_SUCCESS) {
		HWND hwnd = FindWindow(mClassName, mAppName);
		if (hwnd) {
			ShowWindow(hwnd, SW_SHOWNORMAL);
			SetFocus(hwnd);
			SetForegroundWindow(hwnd);
			SetActiveWindow(hwnd);
			return false;
		}
	}
	return true;
}

void Win32::showErrorBox(const wchar_t *message) const
{
	std::wostringstream oss;
	oss << message << L" (error " << GetLastError() << L")";
	MessageBox((hWnd?hWnd:NULL), oss.str().c_str(), L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
}

void Win32::setActive(bool active)
{
	mActive = active;
	//if (active) { engine.resetAfterInactive(); }
}

void Win32::takeCursor()
{
	if (mAppHasMouse) return;
	while (ShowCursor(FALSE) >= 0) {} // guarantee that the cursor will hide
	// clip the cursor to the window's client area
	POINT ulPoint = {0,0};
	ClientToScreen(hWnd, &ulPoint);
	int resX = m_pSettings->resXSet();
	int resY = m_pSettings->resYSet();
	// clip it to the window
	RECT clipRect = {ulPoint.x, ulPoint.y, ulPoint.x + resX, ulPoint.y + resY };
	ClipCursor(&clipRect);
	mAppHasMouse = true;
	//debugPrintf("TAKE CURSOR!\n"); // TEMP
}

void Win32::giveCursor()
{
	if (!mAppHasMouse) return;
	ClipCursor(NULL);	// unclip the cursor
	while (ShowCursor(TRUE) < 0) {} // guarantee that the cursor will show
	mAppHasMouse = false;
	//debugPrintf("GIVE CURSOR!\n"); // TEMP
}

bool Win32::initWindow()
{
	WNDCLASSEX	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	
	// create WindowClass
    wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= LoadIcon(0, IDI_APPLICATION);
	wc.hIconSm			= LoadIcon(0, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(0, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= mClassName;    

	if (!RegisterClassEx(&wc)) {
		showErrorBox(L"Failed to register the window class");
		return false;
	}
	mInitFlags[INIT_WNDCLASS] = true; // set the init flag

	// get screen resolution
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	// set windowed mode settings
	dwExStyle = WS_EX_APPWINDOW;
	dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	
	// clamp the maximum window size to the monitor resolution
	m_pSettings->resX = (m_pSettings->resX > screenX ? screenX : m_pSettings->resX);
	m_pSettings->resY = (m_pSettings->resY > screenY ? screenY : m_pSettings->resY);

	// if the requested client area is full screen, get rid of the title bar and border
	if (m_pSettings->resX == screenX || m_pSettings->resY == screenY) {
		dwStyle = WS_POPUP;
	}

	// find the window RECT size based on desired client area resolution
	RECT wRect = {0, 0, m_pSettings->resXSet(), m_pSettings->resYSet()};
	if (!getWindowSize(wRect, dwStyle, dwExStyle)) {
		showErrorBox(L"Window sizing error");
		return false;
	}
	int wWidth = wRect.right;
	int wHeight = wRect.bottom;

	// create new window
	hWnd = CreateWindowEx(	dwExStyle,
							mClassName,
							mAppName,
							dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							wWidth,
							wHeight,
							0,
							0,
							hInst,
							0);
	if (!hWnd) {
		showErrorBox(L"Window creation error");
		return false;
	}
	mInitFlags[INIT_WINDOW] = true; // set the init flag

	takeCursor();
	ShowWindow(hWnd, SW_SHOWNORMAL);
	SetFocus(hWnd);
	
	return true;
}

void Win32::killWindow()
{
	giveCursor();
	
	if (hWnd && mInitFlags[INIT_WINDOW] && !mInitFlags[DEINIT_WINDOW]) {
		if (!DestroyWindow(hWnd)) {
			showErrorBox(L"Could not release hWnd");
		}
		hWnd = 0;
		mInitFlags[DEINIT_WINDOW] = true; // set the deinit flag
	}

	if (hInst && mInitFlags[INIT_WNDCLASS] && !mInitFlags[DEINIT_WNDCLASS]) {
		if (!UnregisterClass(mClassName, hInst)) {
			showErrorBox(L"Could not unregister class");
		}
		hInst = 0;
		mInitFlags[DEINIT_WNDCLASS] = true; // set the deinit flag
	}
}

bool Win32::getWindowSize(RECT &outRect, DWORD dwStyle, DWORD dwExStyle) const
{
	// find the window RECT size based on desired client area resolution
	if (!AdjustWindowRectEx(&outRect, dwStyle, FALSE, dwExStyle)) {
		return false;
	}
	// pass back the size in right, bottom
	outRect.right = outRect.right - outRect.left;
	outRect.bottom = outRect.bottom - outRect.top;
	outRect.left = outRect.top = 0;
	debugPrintf("Window width=%i  height=%i\n", outRect.right, outRect.bottom);
	return true;
}

bool Win32::checkFullScreenSettings(DISPLAY_DEVICE &dispDevice, DEVMODE &dmScreenSettings)
{
	dispDevice.cb = sizeof(DISPLAY_DEVICE);
	wchar_t *deviceNamePtr = 0;

	dmScreenSettings.dmSize	= sizeof(DEVMODE);
	dmScreenSettings.dmDriverExtra = 0; // if primary display driver cannot be found, null is used
	
	// Choose the primary display driver
	int d = 0;
	for (;;) {			
		if (EnumDisplayDevices(0, d, &dispDevice, 0) == 0) break;

		if ((dispDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
			(dispDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) {
			deviceNamePtr = dispDevice.DeviceName;
			debugPrintf("Display driver \"%S\" chosen\n", dispDevice.DeviceString);
			break;
		}
		++d;
	}

	// Choose the closest matching display settings (the first to match
	// on all 4 criteria will exit early)
	int mostMatches = 0, c = 0;
	for (;;) {
		DEVMODE dmEnum;
		dmEnum.dmSize = sizeof(DEVMODE);
		dmEnum.dmDriverExtra = 0;

		if (EnumDisplaySettings(deviceNamePtr, c, &dmEnum) == 0) break;

		int matches = 0;
		if (dmEnum.dmBitsPerPel == m_pSettings->bpp)    { ++matches; }
		if (dmEnum.dmPelsWidth  == m_pSettings->fsResX) { ++matches; }
		if (dmEnum.dmPelsHeight == m_pSettings->fsResY) { ++matches; }
		if (dmEnum.dmDisplayFrequency == m_pSettings->refreshRate) { ++matches; }

		if (matches > mostMatches) {
			mostMatches = matches;
			memcpy_s(&dmScreenSettings, sizeof(DEVMODE), &dmEnum, sizeof(DEVMODE));
		}
		if (matches == 4) break;

		++c;
	}		

	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFLAGS | DM_DISPLAYFREQUENCY;

	LONG check = ChangeDisplaySettings(&dmScreenSettings, CDS_TEST);
		
	switch (check) {
		case DISP_CHANGE_SUCCESSFUL:
			return true;
			break;
		case DISP_CHANGE_FAILED:
			showErrorBox(L"Failed to set fullscreen mode");
			break;
		case DISP_CHANGE_BADMODE:
			showErrorBox(L"Fullscreen video mode not supported");
			break;
		case DISP_CHANGE_RESTART:
			showErrorBox(L"Must restart to get fullscreen video mode");
			break;
		default:
			showErrorBox(L"Failed to set fullscreen mode");
	}
	return false;
}

bool Win32::setFullScreen()
{
	DISPLAY_DEVICE dispDevice;
	DEVMODE dmScreenSettings;

	bool ok = checkFullScreenSettings(dispDevice, dmScreenSettings);
	if (ok) { // fullscreen mode supported
		// Since DirectX will set fullscreen when initializing the device, we don't actually
		// set it here, only perform the check so we know if the window can initially be set
		// up for fullscreen mode. Fullscreen settings will be altered to the closest match.
		
		//ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		m_pSettings->set(
			m_pSettings->resX,
			m_pSettings->resY,
			dmScreenSettings.dmPelsWidth,
			dmScreenSettings.dmPelsHeight,
			dmScreenSettings.dmBitsPerPel,
			dmScreenSettings.dmDisplayFrequency,
			true, true,
			m_pSettings->vsync
		);
	}
	return ok;
}

/*
bool Win32::toggleFullScreen()
{
	int width = 0, height = 0;
	int posX = mSettings.wndPosX, posY = mSettings.wndPosY;
	HWND hWndInsertAfter = HWND_NOTOPMOST;
	// get the new desired fullscreen settings
	if (mSettings.toggleFullscreen()) {
		// check that fullscreen settings are ok
		if (setFullScreen()) {
			// fullscreen mode settings
			DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;
			DWORD dwStyle = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
			hWndInsertAfter = HWND_TOPMOST;
			SetWindowLongPtr(hWnd, GWL_STYLE, dwStyle);
			SetWindowLongPtr(hWnd, GWL_EXSTYLE, dwExStyle);
			width = mSettings.resXSet();
			height = mSettings.resYSet();
			posX = posY = 0;
		} else {
			debugPrintf("Could not toggle fullscreen!");
			return false;
		}
	// get the desired windowed settings
	} else {
		// windowed mode settings
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW;
		DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
						WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
		SetWindowLongPtr(hWnd, GWL_STYLE, dwStyle);
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, dwExStyle);
		mSettings.fullscreenSet = false;
		RECT wRect = {0, 0, mSettings.resXSet(), mSettings.resYSet()};
		getWindowSize(wRect, dwStyle, dwExStyle);
		width = wRect.right;
		height = wRect.bottom;
	}
	giveCursor();
	// commit the changes to the window
	if (!SetWindowPos(hWnd, hWndInsertAfter,
					  posX, posY,
					  width, height,
					  SWP_FRAMECHANGED | SWP_SHOWWINDOW))
	{
		debugPrintf("SetWindowPos failed!\n");
		return false;
	}
	if (!mSettings.fullscreenSet) {
		ShowWindow(hWnd, SW_SHOWNORMAL);
	}
	// change the display mode accordingly
	
	//if (!engine.mRenderMgr->makeDisplayChanges(	mSettings.bpp,
	//											width,
	//											height,
	//											mSettings.refreshRate,
	//											mSettings.fullscreenSet,
	//											mSettings.vsync))
	//{
	//	return false;
	//}
	
	takeCursor();

	return true;
}
*/

bool Win32::initInputDevices()
{
	// find number of mouse buttons
	int mouseNumBtns = GetSystemMetrics(SM_CMOUSEBUTTONS); // set some persistent value
	if (mouseNumBtns == 0) { // check for mouse existence
		showErrorBox(L"No mouse detected, closing!");
		return false;
	}
	// find if mouse has a vertical scroll wheel
	bool mouseHasWheel = (GetSystemMetrics(SM_MOUSEWHEELPRESENT) != 0); // set some persistent value

	// register the mouse for HID input
	RAWINPUTDEVICE rid[1];
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
	rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
	rid[0].dwFlags = 0; // or RIDEV_INPUTSINK;
	rid[0].hwndTarget = hWnd;
	BOOL result = RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
	if (!result) {
		showErrorBox(L"Failed to register mouse for raw input");
		return false;
	}

	// get keyboard type information
	int kbType = GetKeyboardType(0); // type (1-7), set some persistent value
	if (!kbType) {
		showErrorBox(L"No keyboard detected, closing!");
		return false;
	}
	int kbNumFuncKeys = GetKeyboardType(2); // number of function keys, set some persistent value

	debugPrintf("Keyboard type:%i   func.keys:%i\n", kbType, kbNumFuncKeys);
	debugPrintf("Mouse detected: btns:%i   wheel:%i\n", mouseNumBtns, mouseHasWheel);
	return true;
}

bool Win32::getSystemInformation()
{
	// test the Windows OS version (Vista (6.0) or greater to pass)
	DWORD version = GetVersion();
	DWORD majorVersion = LOBYTE(LOWORD(version));
	DWORD minorVersion = HIBYTE(LOWORD(version));
	DWORD build = 0; if (version < 0x80000000) { build = HIWORD(version); }
	debugPrintf("Windows version %d.%d.%d\n", majorVersion, minorVersion, build);
	if (majorVersion < 6) {
		MessageBox((hWnd?hWnd:NULL), L"This application is not supported on this version of Windows.\nThe minimum required version is 6.0 (Windows Vista 64).",
			L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
		return false;
	}

	// Get the application data path
	// For Vista/7+ use \ProgramData\ to store files
	PWSTR ppszPath = 0;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_ProgramData, KF_FLAG_NO_ALIAS, NULL, &ppszPath);
	if (SUCCEEDED(hr)) {
		// memcpy ppszPath into a persistent variable
		debugWPrintf(L"ProgramData path: \"%s\"\n", ppszPath);
		CoTaskMemFree(ppszPath);
	}

	// Get system info
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	debugPrintf("Hardware information:\n");
	debugPrintf("\tOEM ID: %u\n", siSysInfo.dwOemId);
	debugPrintf("\tNumber of processors: %u\n", siSysInfo.dwNumberOfProcessors);
	debugPrintf("\tPage size: %u\n", siSysInfo.dwPageSize);
	debugPrintf("\tProcessor type: %u\n", siSysInfo.dwProcessorType);
	debugPrintf("\tMinimum application address: %lx\n", siSysInfo.lpMinimumApplicationAddress);
	debugPrintf("\tMaximum application address: %lx\n", siSysInfo.lpMaximumApplicationAddress);
	debugPrintf("\tActive processor mask: %u\n", siSysInfo.dwActiveProcessorMask);

	return true;
}

bool Win32::checkCapabilities()
{
	BOOL result = XMVerifyCPUSupport();
	if (!result) {
		showErrorBox(L"Error: SSE2 instruction set not supported. You must upgrade your CPU!");
		return false;
	}
	return true;
}

LRESULT Win32::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_CLOSE:
			killWindow(); // calls DestroyWindow
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		// handle deactivate and non mouse-click activate
		case WM_ACTIVATE: {
			WORD activeFlag = LOWORD(wParam);
			WORD minimized = HIWORD(wParam);
			if (activeFlag == WA_INACTIVE || minimized) { // lost focus
				setActive(false);
				giveCursor();

				//debugPrintf("LOST FOCUS!\n");

			// gained focus not by mouse click, was not active and not minimized
			} else if (activeFlag == WA_ACTIVE && !isActive() && !minimized) {
				setActive(true);
				takeCursor();

				//debugPrintf("GOT FOCUS!\n");
			}
			break;
		}
		// handle mouse-click activate
		case WM_MOUSEACTIVATE: {
			//debugPrintf("GOT FOCUS FROM MOUSE\n");
			setActive(true);
			// if the mouse caused activation, if click was in the client area
			// hide/clip the cursor, otherwise leave OS cursor visible
			if (LOWORD(lParam) == HTCLIENT) {
				takeCursor();
				return MA_ACTIVATEANDEAT;
			}
			// if user clicks the title bar or border or buttons, don't take the cursor
			return MA_ACTIVATE;
			break;
		}

		case WM_PAINT: {
			// This only gets called when the window is invalidated somehow, like resizing or
			// dragging another window across it. In any case, the app should be inactive at that
			// time so this check is just for good measure, so it's clear that you aren't really
			// calling render twice in one game loop (render is called during app idle processing).
			
			/*if (!isActive()) { if (Engine::exists()) engine.render(); }*/
			
			return DefWindowProc(hwnd, msg, wParam, lParam);
			
			/*PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);*/
			break;
		}

		case WM_DISPLAYCHANGE:
			debugPrintf("Display mode changed: resX=%i  resY=%i  bpp=%u\n", LOWORD(lParam), HIWORD(lParam), wParam);
			break;

		case WM_MOVE:
			if (!m_pSettings->fullscreenSet) {
				m_pSettings->wndPosX = LOWORD(lParam);	// horizontal position
				m_pSettings->wndPosY = HIWORD(lParam);	// vertical position
			}
			break;

		case WM_SIZE:
			//SIZE_MINIMIZED
				//Pause rendering
			//SIZE_RESTORED
				//If a SIZE_MINIMIZED previously occurred then un-pause rendering
				//If between a WM_ENTERSIZEMOVE and WM_EXITSIZEMOVE then ignore
				//Otherwise, check for the host window changing size. If the window has changed size reset the device
			//SIZE_MAXIMISED
				//Un-pause rendering
			break;

		case WM_ENTERSIZEMOVE:
			// Pause rendering until the user finishes resizing the target window
			setActive(false);
			debugPrintf("WM_ENTERSIZEMOVE\n");
			break;

		case WM_EXITSIZEMOVE:
			// Un-pause rendering
			// Check for the host window changing size; if the window has changed then reset the device
			setActive(true);
			debugPrintf("WM_EXITSIZEMOVE\n");
			break;

		// Handle Mouse Input
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_MBUTTONDOWN:
			break;
		case WM_MBUTTONUP:
			break;
		case WM_XBUTTONDOWN: {
			int fwButton = GET_XBUTTON_WPARAM(wParam);
			debugPrintf("WM_XBUTTONDOWN %i\n", fwButton);
			break;
		}
		case WM_XBUTTONUP: {
			int fwButton = GET_XBUTTON_WPARAM(wParam);
			debugPrintf("WM_XBUTTONUP %i\n", fwButton);
			break;
		}
		case WM_MOUSEWHEEL: {
			int fwKeys = GET_KEYSTATE_WPARAM(wParam);
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			debugPrintf("WM_MOUSEWHEEL %i\n", zDelta);
			break;
		}
		case WM_MOUSEHWHEEL: {
			int fwKeys = GET_KEYSTATE_WPARAM(wParam);
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			debugPrintf("WM_MOUSEHWHEEL %i\n", zDelta);
			break;
		}

		// WM_MOUSEMOVE is used for cursor control because pointer ballistics apply.
		// It is not used for high-precision first-person camera or similar control.
		case WM_MOUSEMOVE: {
			int posX = LOWORD(lParam);
			int posY = HIWORD(lParam);
			break;
		}

		// WM_INPUT is used for high-precision mouse movements such as camera control,
		// it is not used for cursor control since pointer ballistics do not apply. It
		// can also capture Joystick and other controller raw inputs, but DirectInput
		// may be used for those.
		case WM_INPUT: {
			UINT dwSize = 40;		// This hard-coded size is specific to mouse.
			static BYTE lpb[40];	// Consider using variable size like below.

			/* or...
			UINT dwSize;
			// get the required size of the data buffer
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, 
							sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (!lpb) return 0;
			*/
			
			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize,
								sizeof(RAWINPUTHEADER)) != dwSize)
			{
				//debugPrintf("WM_INPUT: GetRawInputData doesn't return the expected size!\n");
			}

			RAWINPUT &raw = *((RAWINPUT*)lpb);

			if (raw.header.dwType == RIM_TYPEMOUSE) {
				// mouse snooper
				/*debugPrintf("Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x\n",
							raw.data.mouse.usFlags,
							raw.data.mouse.ulButtons,
							raw.data.mouse.usButtonFlags,
							raw.data.mouse.usButtonData,
							raw.data.mouse.ulRawButtons,
							raw.data.mouse.lLastX,
							raw.data.mouse.lLastY,
							raw.data.mouse.ulExtraInformation);*/
			}
			break;
		}

		// Handle Keyboard Input
		// Alt key handling
		case WM_SYSKEYDOWN:
			switch (wParam) {
				// handle fullscreen toggle Alt-Enter
			//	case VK_RETURN:
					//DWORD dwMask = (1 << 29);
                    //if ((lParam & dwMask) != 0) { // Alt is down also
			//		toggleFullScreen();
					//}
			//		break;
				// if the alt key is pressed down don't send a WM_SYSCOMMAND message
				case VK_MENU:
					break;
				default:
					return DefWindowProc(hwnd, msg, wParam, lParam);
			}
			break;
		case WM_SYSKEYUP:
			switch (wParam) {
				// if the alt key is upped, in windowed mode toggle the cursor
				case VK_MENU:
					if (!m_pSettings->fullscreenSet) {
						if (mAppHasMouse && isActive()) {
							giveCursor();
						} else {
							takeCursor();
						}
					}
					break;
				default:
					return DefWindowProc(hwnd, msg, wParam, lParam);
			}
			break;

		// handle other keys
		case WM_CHAR:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			// keyboard snooper
			UINT extended	= ((HIWORD(lParam) & KF_EXTENDED) != 0);
			UINT altDown	= ((HIWORD(lParam) & KF_ALTDOWN) != 0);
			UINT repeat		= ((HIWORD(lParam) & KF_REPEAT) != 0);
			UINT up			= ((HIWORD(lParam) & KF_UP) != 0);
			UINT repeatCnt	= LOWORD(lParam);
			UINT vsc		= LOBYTE(HIWORD(lParam));
			UINT vk			= MapVirtualKey(vsc, MAPVK_VSC_TO_VK_EX);
			
			#ifdef DEBUG_CONSOLE
			//	if		(msg == WM_KEYDOWN)	{ debugPrintf("WM_KEYDOWN "); }
			//	else if (msg == WM_CHAR)	{ debugPrintf("WM_CHAR    "); }
			//	else if (msg == WM_KEYUP)	{ debugPrintf("WM_KEYUP   "); }
			#endif
			
			//debugPrintf("code=%3u '%c' vk=%3u oem=%2u ext'd=%u altDn=%u rpt=%u rptCnt=%u rel'd=%u\n",
			//			wParam, (wParam>31&&wParam<128)?wParam:' ', vk, vsc, extended, altDown, repeat, repeatCnt, up);
			
			switch (wParam) {
				case VK_ESCAPE: {
					win32.exit();
					break;
				}
			}
			break;
		}

		case WM_SYSCOMMAND:
			switch (wParam & 0xFFF0) {
				// these are ignored and not handled
				case SC_SCREENSAVE:
					// Microsoft Windows Vista and later: If password protection is enabled by policy,
					// the screen saver is started regardless of what an application does with the
					// SC_SCREENSAVE notification even if fails to pass it to DefWindowProc.
				case SC_MONITORPOWER:
				case SC_HSCROLL:
				case SC_VSCROLL:
				case SC_KEYMENU:
				case SC_HOTKEY:
				case SC_CONTEXTHELP:
					break;
				// all others use the default handler
				default:
					return DefWindowProc(hwnd, msg, wParam, lParam);
			}
			break;

		// ignore these messages (return 0)
		case WM_CREATE:
		case WM_APPCOMMAND:
		case WM_ERASEBKGND:
			break;

		// all other messages use default handler
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

bool Win32::initConsole()
{
	BOOL result = AllocConsole();
	if (result) {
		result = SetConsoleCtrlHandler((PHANDLER_ROUTINE)Win32::ctrlHandler, TRUE);
		if (result) {
			FILE *ofs;
			freopen_s(&ofs, "CONOUT$", "w", stdout);
		} else {
			showErrorBox(L"Could not set console handler");
		}
	} else {
		showErrorBox(L"Could not create debug console window");
	}
	bool success = (result != 0);
	if (success) {
		HWND hWnd_c = GetConsoleWindow();
		SetConsoleTitle(L"Icarus Debugging Console");
		SetWindowPos(hWnd_c, 0, CONSOLE_X, CONSOLE_Y,
					 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
		// set max number of lines in console
		CONSOLE_SCREEN_BUFFER_INFO sbInfo;
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(hOut, &sbInfo);
		sbInfo.dwSize.Y = CONSOLE_MAX_LINES;
		SetConsoleScreenBufferSize(hOut, sbInfo.dwSize);
		// set window size
		MoveWindow(hWnd_c, CONSOLE_X, CONSOLE_Y, 800, 800, TRUE);
	}
	return success;
}

BOOL WINAPI Win32::ctrlHandler(DWORD dwCtrlType) 
{
	switch (dwCtrlType) {
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
			FreeConsole();
			return TRUE;

		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			PostMessage(win32.hWnd, WM_CLOSE, 0, 0);
			return TRUE;

		default:
			return FALSE;
	}
}

// Constructor / destructor
void Win32::init(HINSTANCE hInstance, Settings *pSettings)
{
	hInst = hInstance;
	m_pSettings = pSettings;
}

Win32::Win32(const wchar_t *appName, const wchar_t *className) :
	mActive(true),
	mExit(false),
	mAppHasMouse(false),
	mAppName(appName),
	mClassName(className),
	m_pSettings(0),
	mInitFlags(0),
	hInst(0),
	hWnd(0),
	hMutex(0)
{}

Win32::~Win32()
{
	killWindow();
	if (hMutex) ReleaseMutex(hMutex);
}