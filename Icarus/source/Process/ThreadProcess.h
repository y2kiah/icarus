/* ThreadProcess.h
Author: Jeff Kiah
Orig.Date: 05/30/2012
*/
#pragma once

#include "Process.h"
#include <string>
#include <boost/thread/thread.hpp>

using std::string;
using boost::thread;

///// STRUCTURES /////

/*=============================================================================
class ThreadProcess
=============================================================================*/
class ThreadProcess : public Process {
	private:
		thread			mThread;
		thread::id		mThreadID;
		volatile bool	mKillThread;	// set true to request the thread to shutdown

	protected:

		/*---------------------------------------------------------------------
			This function runs in the main thread, called by the process
			manager as with all attached processed. It can be used to check for
			output from the threaded process. It can also call killThread() to
			request the thread to shut itself down. The thread will exit as a
			response to killThread(), and the main thread should call finish(),
			which calls onFinish() which waits for the join() to return. Don't
			call finish() from the thread process, that would cause a deadlock.
		---------------------------------------------------------------------*/
		virtual void onUpdate(double deltaMillis) = 0;

		/*---------------------------------------------------------------------
			This begins thread execution by contructing a new thread in the
			member variable, passing (this) as data.
			**NOTE**
			A derived class can override this function if necessary, but MUST
			explicitly call this base class version of the function within it,
			or perform the same tasks in its own implementation.
		---------------------------------------------------------------------*/
		virtual bool onInitialize(); // return false if initialization fails, finishes process

		/*---------------------------------------------------------------------
			This can be overridden, but the derived version MUST explicitly
			call this base class version or perform the same tasks within its
			own implementation. Calling join() as a response to a call to
			finish() ensures no orphaned threads. Make sure you call finish()
			from the main thread, not the worker thread, to avoid deadlock.
		---------------------------------------------------------------------*/
		inline virtual void	onFinish();

		/*---------------------------------------------------------------------
			This could potentially be supported by a combination of the thread
			interrupt feature and a condition variable.
		---------------------------------------------------------------------*/
		inline virtual void onTogglePause();

		/*---------------------------------------------------------------------
			Call this from the main thread to request the worker thread to shut
			itself down by returning. The main thread should then call the
			finish() method to wait for the thread to join().
		---------------------------------------------------------------------*/
		void killThread() { mKillThread = true; }
		bool threadKilled() const { return mKillThread; }

		/*---------------------------------------------------------------------
			Call these from within threadProc to yield time to other threads,
			or sleep for a given length of time.
		---------------------------------------------------------------------*/
		void yield() const { mThread.yield(); }
		//void sleep() const { mThread.sleep(); }

		/*---------------------------------------------------------------------
			This is the function to run in the new thread. It should monitor
			threadKilled() and shut itself down when requested by the main
			thread. It may also end by returning from the function, but should
			be sure to call killThread() before exiting to notify the main
			thread that it has finished. The main thread would then respond to
			threadKilled() by calling finish() and waiting for the thread to
			join(), then the ThreadProcess object could be destroyed.
		---------------------------------------------------------------------*/
		virtual void threadProc() = 0;

	public:
		thread::id threadID() const { return mThreadID; }

		// Constructor / destructor
		explicit ThreadProcess(const string &name);
		virtual ~ThreadProcess();
};

#include "Impl/ThreadProcess.inl"