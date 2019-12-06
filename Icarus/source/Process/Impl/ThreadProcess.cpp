/* ThreadProcess.h
Author: Jeff Kiah
Orig.Date: 06/09/2012
*/
#include "Process/ThreadProcess.h"

////////// class ThreadProcess //////////

/*---------------------------------------------------------------------
	This begins thread execution by contructing a new thread in the
	member variable, passing (this) as data.
	**NOTE**
	A derived class can override this function if necessary, but MUST
	explicitly call this base class version of the function within it,
	or perform the same tasks in its own implementation.
---------------------------------------------------------------------*/
bool ThreadProcess::onInitialize() // return false if initialization fails, finishes process
{
	mThread = thread(&ThreadProcess::threadProc, this);
	mThreadID = mThread.get_id();
	debugPrintf("ThreadProcess: \"%s\" started\n", name().c_str());
	return true;
}

// Constructor
ThreadProcess::ThreadProcess(const string &name) :
	Process(name, Process_Run_Frame, Process_Queue_Single), // although async in nature, use run_frame because onUpdate must run every frame
	mThread(), // construct a Not-a-Thread object, onInitialize() will swap in the running thread
	mThreadID(mThread.get_id()),
	mKillThread(false)
{}

// Destructor
ThreadProcess::~ThreadProcess()
{
	if (!isFinished()) {
		debugPrintf("ThreadProcess: \"%s\" destructor: finish() not called. Possible orphaned thread!\n", name().c_str());
	}
}
