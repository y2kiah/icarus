/* ProcessManager.cpp
Author: Jeff Kiah
Orig.Date: 06/09/2012
*/

#include "Process/ProcessManager.h"
#include "Process.h"


bool ProcessManager::isProcessActive(const string &procName)
{
	auto end = mProcessList.end();
	for (auto i = mProcessList.begin(); i != end; ++i) {
		if (_stricmp((*i)->name().c_str(), procName.c_str()) == 0) return true;
	}
	return false;
}

void ProcessManager::attach(const ProcessPtr &procPtr)
{
	mProcessList.push_back(procPtr);
	procPtr->setAttached();
}

void ProcessManager::detach(const ProcessPtr &procPtr)
{
	procPtr->setAttached(false);
	debugPrintf("ProcessManager: \"%s\" process detached: %i running\n", procPtr->name().c_str(), mProcessList.size()-1);
	mProcessList.remove(procPtr);
}

void ProcessManager::updateProcesses(double deltaMillis)
{
	auto i = mProcessList.begin(), end = mProcessList.end();

	while (i != end) {
		ProcessPtr &p = (*i);
		++i;
		
		if (p->isFinished()) {
			detach(p);
		} else {
			p->update(deltaMillis);
		}
	}
}

/*---------------------------------------------------------------------
	destroys all processes in the list
---------------------------------------------------------------------*/
void ProcessManager::clear()
{
	mProcessList.clear();
}
