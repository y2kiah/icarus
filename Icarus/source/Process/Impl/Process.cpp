/* Process.cpp
Author: Jeff Kiah
Orig.Date: 06/17/2012
*/
#include "Process/ProcessManager.h"
#include "Utility/Debug.h"

// class Process
/*---------------------------------------------------------------------
	This function is called to perform the main task of the process. If
	the process has not been initialized, it will be prior to calling
	onUpdate() for the first time.
---------------------------------------------------------------------*/
void Process::update(double deltaMillis)
{
	if (!isActive() || isPaused()) { return; }

	if (!isInitialized()) {
		if (!onInitialize()) {
			finish();
			return;
		}
		setInitialized();
	}
	onUpdate(deltaMillis);
}

// class ProcessChain
/*---------------------------------------------------------------------
	A derived class must explicitly call this base class method, or
	implement the same functionality on its own.
---------------------------------------------------------------------*/
void ProcessChain::onUpdate(double deltaMillis)
{
	ProcessPtr &p = m_process[m_current];
	p->update(deltaMillis);
	afterOneUpdate(p);
			
	// if the current process is finished, move on to the next one
	if (p->isFinished()) {
		afterOneFinish(p);
				
		// if this was the last process, finish the chain
		if (m_current == m_process.size() - 1) {
			finish();
		} else {
			++m_current;
		}
	}
}

// class ProcessGroup
/*---------------------------------------------------------------------
	A derived class must explicitly call this base class method, or
	implement the same functionality on its own.
---------------------------------------------------------------------*/
void ProcessGroup::onUpdate(double deltaMillis)
{
	bool allFinished = true;
	for (auto i = m_process.begin(); i != m_process.end(); ++i) {
		Process *p = i->get();
		if (!p->isFinished()) {
			beforeOneUpdate(*i);
			p->update(deltaMillis);
			afterOneUpdate(*i);
			if (p->isFinished()) { afterOneFinish(*i); }
		}
		allFinished = allFinished && p->isFinished();
	}
	// if all processes are finished, finish this group
	if (allFinished) { finish(); }
}