/* ThreadProcess.inl
Author: Jeff Kiah
Orig.Date: 05/30/2012
*/
#pragma once

#include "Process/ThreadProcess.h"
#include "Utility/Debug.h"

/*---------------------------------------------------------------------
	This can be overridden, but the derived version MUST explicitly
	call this base class version or perform the same tasks within its
	own implementation. Calling join() as a response to a call to
	finish() ensures no orphaned threads. Make sure you call finish()
	from the main thread, not the worker thread, to avoid deadlock.
---------------------------------------------------------------------*/
inline void	ThreadProcess::onFinish()
{
	if (mThread.joinable()) mThread.join();
	debugPrintf("ThreadProcess: \"%s\" onFinish called\n", name().c_str());
}

/*---------------------------------------------------------------------
	This could potentially be supported by a combination of the thread
	interrupt feature and a condition variable.
---------------------------------------------------------------------*/
inline void ThreadProcess::onTogglePause()
{
	_ASSERTE(false && "Pause not supported on thread processes");
}
