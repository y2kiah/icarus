/* Timer.h
Author: Jeff Kiah
Orig.Date: 5/21/12
*/
#pragma once

#include <cstdint>

#define DISCREPANCY_MS_CHECK	2

///// STRUCTURES /////

// class Timer

class Timer {
	private:
		// Static Variables
		static int64_t	sTimerFreq;
		static double	sSecondsPerCount;
		static double	sMillisecondsPerCount;
		
		#ifdef _DEBUG
		static bool		sInitialized;	// in debug mode asserts make sure static functions aren't called before init
		#endif

		// Member Variables
		int64_t			mStartCounts, mStopCounts, mCountsPassed;	// high res timer, set by QPC on windows
		//uint64_t		mStartTickCount, mStopTickCount;			// low res timer, set by GetTickCount64 on windows to check for QPC jumps
		double			mMillisecondsPassed;
		double			mSecondsPassed;

	public:
		// Static Functions
		#ifdef _DEBUG
		static bool		initialized();
		#endif
		static int64_t	timerFreq();
		static double	secondsPerCount();
		static int64_t	queryCounts();
		static int64_t	countsSince(int64_t startCounts);
		static double	secondsSince(int64_t startCounts);
		static double	secondsBetween(int64_t startCounts, int64_t stopCounts);
		static bool		initHighPerfTimer();

		// Functions
		inline int64_t	startCounts() const;
		inline int64_t	stopCounts() const;
		inline int64_t	countsPassed() const;
		inline double	millisecondsPassed() const;
		inline double	secondsPassed() const;

		void			start();
		double			stop();
		inline void		reset();
		inline int64_t	currentCounts() const;
		inline double	currentSeconds() const;

		// Constructor
		explicit Timer();
};

#if defined(WIN32)
#include "Impl/Timer_win32.inl"
#endif
