/* Win32.h
Author: Jeff Kiah
Started: 5/16/12
*/
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// defined in project settings
#endif

#include <windows.h>
#include <bitset>

using std::bitset;

// Declarations

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Classes

class Settings;

class Platform {
	public:
		virtual void showErrorBox(const wchar_t *) const = 0;
		virtual bool initWindow() = 0;
		virtual bool initInputDevices() = 0;

		virtual ~Platform() {}
};

class Win32 : public Platform {
	private:
		// Variables
		bool mActive;		// true if this is the active window
		bool mExit;			// true if the app has been flagged to exit
		bool mAppHasMouse;	// true if app has mouse control, false if Windows cursor visible
		const wchar_t *mAppName;
		const wchar_t *mClassName;
		Settings *m_pSettings;

		// initialization flags
		enum Win32InitFlags {
			INIT_WINDOW = 0,
			DEINIT_WINDOW,
			INIT_WNDCLASS,
			DEINIT_WNDCLASS,
			INIT_MAX
		};
		bitset<INIT_MAX> mInitFlags;

	public:
		HINSTANCE	hInst;
		HWND		hWnd;
		HANDLE		hMutex;

		// Functions

		// misc functions
		bool isOnlyInstance();
		bool isActive() const { return mActive; }
		bool isExiting() const { return mExit; }
		void setActive(bool active = true);	// set and unset app active status
		void exit() { mExit = true; } // call to initiate a graceful exit
		void takeCursor();	// takes control of the mouse cursor from Windows
		void giveCursor();	// gives mouse control back to Windows
		virtual void showErrorBox(const wchar_t *message) const;

		// main window
		virtual bool initWindow();
		virtual bool initInputDevices();
		void killWindow();
		bool checkFullScreenSettings(DISPLAY_DEVICE &dispDevice, DEVMODE &dmScreenSettings);
		bool getWindowSize(RECT &outRect, DWORD dwStyle, DWORD dwExStyle) const;
		bool setFullScreen();
		//bool toggleFullScreen();
		bool getSystemInformation();
		bool checkCapabilities();
		LRESULT wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		// debug console
		bool initConsole();
		static BOOL WINAPI ctrlHandler(DWORD dwCtrlType);

		// Constructor / Destructor
		void init(HINSTANCE hInstance, Settings *pSettings);
		explicit Win32(const wchar_t *appName, const wchar_t *className);
		virtual ~Win32();
};