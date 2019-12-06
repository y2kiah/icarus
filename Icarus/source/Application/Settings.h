/* Settings.h
Author:		Jeff Kiah
Orig.Date:	6/20/2009
*/
#pragma once

#include <string>

using std::string;

// Structures

class Settings {
	public:
		int resX, resY;			// windowed mode client area resolution
		int fsResX, fsResY;		// fullscreen resolution
		int	wndPosX, wndPosY;	// windowed-mode window position
		int	bpp;				// fullscreen bits per pixel
		int	refreshRate;		// fullscreen refresh rate
		bool fullscreen;		// true if fullscreen mode desired
		bool fullscreenSet;		// true if fullscreen mode actually set, false if windowed
		bool vsync;				// applies to fullscreen mode only

		string dataDir;			// example "data/"

		int	resXSet() const	{ return (fullscreenSet ? fsResX : resX); }
		int	resYSet() const	{ return (fullscreenSet ? fsResY : resY); }

		inline void	set(int _resX, int _resY, int _fsResX, int _fsResY, int _bpp,
						int _refreshRate, bool _fullscreen, bool _fullscreenSet, bool _vsync);

		bool toggleFullscreen() {
			// We always want to toggle from the actual state of the window, which is
			// help in fullscreenSet. The fullscreen variable contains our desired setting.
			// These two would be different if fullscreen mode failed to set for some reason.
			fullscreen = !fullscreenSet;
			return fullscreen;
		}

		explicit Settings() :
			resX(1280), resY(800),
			fsResX(1680), fsResY(1050),
			bpp(32),
			refreshRate(60),
			fullscreen(false), fullscreenSet(false),
			vsync(true),
			dataDir("data/")
		{}
		~Settings() {}
};

// Inline Functions

inline void Settings::set(int _resX, int _resY, int _fsResX, int _fsResY, int _bpp,
						  int _refreshRate, bool _fullscreen, bool _fullscreenSet, bool _vsync)
{
	resX = _resX;
	resY = _resY;
	fsResX = _fsResX;
	fsResY = _fsResY;
	bpp = _bpp;
	refreshRate = _refreshRate;
	fullscreen = _fullscreen;
	fullscreenSet = _fullscreenSet;
	vsync = _vsync;
}
