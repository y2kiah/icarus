/* Timer_win32.cpp
Author: Jeff Kiah
Orig.Date: 5/21/12
*/
#include "Application/Timer.h"

#if defined(WIN32)

#include "Utility/Debug.h"
#include <cmath>

// Static Variables

int64_t Timer::sTimerFreq = 0;
double Timer::sSecondsPerCount = 0;
double Timer::sMillisecondsPerCount = 0;
#ifdef _DEBUG
bool Timer::sInitialized = false;
#endif

// Static Functions

#ifdef _DEBUG
bool Timer::initialized() { return sInitialized; }
#endif

int64_t Timer::timerFreq()
{
	return sTimerFreq;
}

double Timer::secondsPerCount()
{
	return sSecondsPerCount;
}

int64_t Timer::queryCounts()
{
	_ASSERTE(sInitialized);
	int64_t now = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&now);
	return now;
}

int64_t Timer::countsSince(int64_t startCounts)
{
	_ASSERTE(sInitialized);
	int64_t now = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&now);
	return now - startCounts;
}

double Timer::secondsSince(int64_t startCounts)
{
	_ASSERTE(sInitialized);
	int64_t now = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&now);
	return static_cast<double>(now - startCounts) * sSecondsPerCount;
}

double Timer::secondsBetween(int64_t startCounts, int64_t stopCounts)
{
	_ASSERTE(sInitialized);
	return static_cast<double>(stopCounts - startCounts) * sSecondsPerCount;
}

bool Timer::initHighPerfTimer()
{
	SetThreadAffinityMask(GetCurrentThread(), 1);

	// get high performance counter frequency
	BOOL result = QueryPerformanceFrequency((LARGE_INTEGER *)&sTimerFreq);
	if (result == 0 || sTimerFreq == 0) {
		debugPrintf("Timer::initTimer: QueryPerformanceFrequency failed (error %d)\n", GetLastError());
		return false;
	}

	sSecondsPerCount = 1.0 / static_cast<double>(sTimerFreq);
	sMillisecondsPerCount = sSecondsPerCount * 1000.0;

	// test counter function
	int64_t dummy = 0;
	result = QueryPerformanceCounter((LARGE_INTEGER *)&dummy);
	if (result == 0) {
		debugPrintf("Timer::initTimer: QueryPerformanceCounter failed (error %d)\n", GetLastError());
		return false;
	}

	#ifdef _DEBUG
	sInitialized = true;
	#endif

	return true;
}

// Member Functions

void Timer::start()
{
	_ASSERTE(sInitialized);
	QueryPerformanceCounter((LARGE_INTEGER *)&mStartCounts);
	//mStartTickCount = GetTickCount64();
	mStopCounts = mStartCounts;
	//mStopTickCount = mStartTickCount;
	mCountsPassed = 0;
	mMillisecondsPassed = 0;
	mSecondsPassed = 0;
}

double Timer::stop()
{
	_ASSERTE(sInitialized);
	// query the current counts from QPC and GetTickCount64
	QueryPerformanceCounter((LARGE_INTEGER *)&mStopCounts);
	//mStopTickCount = GetTickCount64();
	// get time passed since start() according to QPC and GetTickCount64
	mCountsPassed = mStopCounts - mStartCounts;
	//int64_t ticksPassed = mStopTickCount - mStartTickCount;
	// find the difference between the two clocks
	//double diff = mMillisecondsPassed - ticksPassed;
	//if (abs(diff) > DISCREPANCY_MS_CHECK) { // check for discrepancy > X ms
		// if the discrepancy is large, QPC probably skipped so we should trust GetTickCount64
	//	debugPrintf("Timer::stop: QPC discrepency detected (difference %fms)\n", diff);
	//	mMillisecondsPassed = static_cast<double>(ticksPassed);
	//	mSecondsPassed = ticksPassed * 0.001;
	//} else {
	mSecondsPassed = min(static_cast<double>(mCountsPassed) * sSecondsPerCount, 0.0);
	mMillisecondsPassed = mSecondsPassed * 1000.0;
	//}
	return mMillisecondsPassed;
}

#endif // ifdef WIN32