/* Application.h
Author: Jeff Kiah
Orig.Date: 06/21/2012
*/
#pragma once

#include <memory>
#include <string>
#include "Render/Renderer.h"

using std::shared_ptr;
using std::unique_ptr;
using std::string;

///// STRUCTURES /////

class Platform;
class Timer;
class Settings;
class EventManager;
class EngineEventListener;
class ProcessManager;
class ResCacheManager;
class ScriptManager;

typedef shared_ptr<Timer>			TimerPtr;
typedef shared_ptr<Settings>		SettingsPtr;
typedef shared_ptr<EventManager>	EventManagerPtr;
typedef shared_ptr<ProcessManager>	SchedulerPtr;
typedef shared_ptr<ResCacheManager>	ResCacheManagerPtr;
typedef shared_ptr<ScriptManager>	ScriptManagerPtr;

/*=============================================================================
class Application
=============================================================================*/
class Application {
	private:
		///// VARIABLES /////
		const Platform *		m_pPlatform;
		const Settings *		m_pSettings;
		TimerPtr				mFrameTimer;
		EventManagerPtr			mEventMgr;
		SchedulerPtr			mScheduler;
		ResCacheManagerPtr		mResCacheMgr;
		ScriptManagerPtr		mScriptMgr;
		RendererPtr				mRenderer;
		
		int		mPausedCount; // pause() increments, unpause() decrements, always >= 0, unpaused when 0
		bool	mExit;

	public:
		const string &appName; // application name used for testing and logging

		///// FUNCTIONS /////
		// Accessors
		bool isPaused() const		{ return (mPausedCount > 0); }
		bool isExiting() const		{ return mExit; }
		const RendererPtr & getRenderer() const { return mRenderer; }

		// Mutators
		void exit()		{ mExit = true; }
		int	 pause()	{ return ++mPausedCount; }
		int	 unpause()	{ return (mPausedCount > 0 ? --mPausedCount : 0); }
		void processFrame();
		void update(double deltaMillis);
		void render();
		void resetAfterInactive();

		void cleanup();

		// Constructor / Destructor
		explicit Application(const string &name,
							 const Platform *pPlatform,
							 const Settings *pSettings,
							 const TimerPtr &timer,
							 const EventManagerPtr &eventMgr,
							 const SchedulerPtr &scheduler,
							 const ResCacheManagerPtr &resCacheMgr,
							 const ScriptManagerPtr &scriptMgr,
							 const RendererPtr &renderer
							);
		~Application();
};

typedef unique_ptr<Application> ApplicationUniquePtr;

/*=============================================================================
class ApplicationFactory
	This is the factory responsible for creating and wiring the dependency
	graph for the application layer of the game.
=============================================================================*/
class ApplicationFactory {
	private:
		const Platform *m_pPlatform;
		const Settings *m_pSettings;

	public:
		ApplicationUniquePtr	createIcarus();

		explicit ApplicationFactory(const Platform *pPlatform, const Settings *pSettings) :
			m_pPlatform(pPlatform), m_pSettings(pSettings)
		{}
};