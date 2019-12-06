/* Process.inl
Author: Jeff Kiah
Orig.Date: 07/13/2012
*/
#pragma once

#include "Process/Process.h"

// class Process

inline bool Process::isFinished() const	{
	return mProcessFlags[bitFinished];
}

void Process::finish() {
	if (!isFinished()) {
		mProcessFlags[bitFinished] = true;
		onFinish();
	}
}

inline bool Process::isActive() const {
	return mProcessFlags[bitActive];
}

inline void Process::setActive(bool b) {
	mProcessFlags[bitActive] = b;
}

inline bool Process::isAttached() const {
	return mProcessFlags[bitAttached];
}

inline void Process::setAttached(bool b) {
	mProcessFlags[bitAttached] = b;
}

inline bool Process::isPaused() const {
	return mProcessFlags[bitPaused];
}

inline void Process::togglePause() {
	mProcessFlags.flip(bitPaused); onTogglePause();
}
		
inline bool Process::isInitialized() const {
	return mProcessFlags[bitInitialized];
}

inline void Process::setInitialized() {
	mProcessFlags[bitInitialized] = true;
}
		
inline const string & Process::name() const {
	return mName;
}

inline Process::Process(const string &name,
						ProcessRunMode runMode,
						ProcessQueueMode queueMode) :
	mName(name), mRunMode(runMode), mQueueMode(queueMode),
	mProcessFlags(0)
{
	mProcessFlags[bitActive] = true;
}

// class ProcessChain
inline size_t ProcessChain::size() const {
	return m_process.size();
}

inline size_t ProcessChain::appendProcess(const ProcessPtr &p) {
	if (isFinished()) { return 0; }
	m_process.push_back(p);
	return size();
}

// class ProcessGroup
inline size_t ProcessGroup::size() const {
	return m_process.size();
}

inline size_t ProcessGroup::addProcess(const ProcessPtr &p) {
	if (isFinished()) { return 0; }
	m_process.push_back(p);
	return size();
}
