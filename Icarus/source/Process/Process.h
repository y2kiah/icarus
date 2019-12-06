/* Process.h
Author: Jeff Kiah
Orig.Date: 05/30/2012
*/
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <bitset>
#include <boost/noncopyable.hpp>

using std::string;
using std::vector;
using std::list;
using std::shared_ptr;
using std::weak_ptr;
using std::bitset;

// ** NOTE **
// The settings controlled by these enums are not implemented yet

enum ProcessQueueMode : uint8_t {
	Process_Queue_Multiple = 0,		// will allow multiple processes of same type in the list
	Process_Queue_Single,			// only allow 1 process of type in the list at a time
	Process_Queue_Single_Replace	// upon submission, will end and replace a process of same type
};

enum ProcessRunMode : uint8_t {
	Process_Run_Frame = 0,			// will run the process every frame regardless of time
	Process_Run_Async				// if max frame time exceeded, will run in a later frame
};

class Process;
class ProcessManager;
typedef shared_ptr<Process>		ProcessPtr;
typedef vector<ProcessPtr>		ProcessList;
typedef list<ProcessPtr>		ProcessLinkedList;
typedef shared_ptr<ProcessManager>	SchedulerPtr;
typedef weak_ptr<ProcessManager>	SchedulerWeakPtr;

/*=============================================================================
class Process
=============================================================================*/
class Process : private boost::noncopyable {
	protected:
		///// VARIABLES /////
		bitset<5>			mProcessFlags;
		ProcessRunMode		mRunMode;
		ProcessQueueMode	mQueueMode;
		const string		mName;

		///// DEFINITIONS /////
		enum CProcessFlagBits : uint8_t {
			bitFinished = 0,
			bitActive,
			bitPaused,
			bitInitialized,
			bitAttached
		};

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			As part of a cooperative multitasking system, these functions are
			responsible for returning control to the main loop without hogging
			CPU. These are pure virtual and must be overridden in derived
			classes.
		---------------------------------------------------------------------*/
		virtual void onUpdate(double deltaMillis) = 0;
		virtual bool onInitialize() = 0; // return false if initialization fails, finishes process
		virtual void onFinish() = 0;
		virtual void onTogglePause() = 0;

	public:
		/*---------------------------------------------------------------------
			This function is called to perform the main task of the process. If
			the process has not been initialized, it will be prior to calling
			onUpdate() for the first time.
		---------------------------------------------------------------------*/
		void update(double deltaMillis);

		// Getters and setters
		inline bool isFinished() const;
		inline void finish();

		inline bool isActive() const;
		inline void setActive(bool b = true);

		inline bool isAttached() const;
		inline void setAttached(bool b = true);

		inline bool isPaused() const;
		inline void togglePause();
		
		inline bool isInitialized() const;
		inline void setInitialized();
		
		inline const string & name() const;

		// Constructor / destructor
		inline explicit Process(const string &name,
								ProcessRunMode runMode = Process_Run_Async,
								ProcessQueueMode queueMode = Process_Queue_Multiple);
		virtual ~Process() {}
};


/*=============================================================================
class ProcessChain
	Serialize processes, one must complete before the next will run. Entire
	chains can be appended to create a single longer chain. A chain is created
	from individual processes, chains, groups, or any combination thereof.
=============================================================================*/
class ProcessChain : public Process {
	protected:
		ProcessList	m_process;
		int			m_current;

		/*---------------------------------------------------------------------
			A derived class must explicitly call this base class method, or
			implement the same functionality on its own.
		---------------------------------------------------------------------*/
		virtual void onUpdate(double deltaMillis);

		// hooks for this parent process to update member data based on subprocess results
		virtual void afterOneUpdate(const ProcessPtr &p) {}
		virtual void afterOneFinish(const ProcessPtr &p) {}

	public:
		// Accessors
		inline size_t size() const;
		// Mutators
		inline size_t appendProcess(const ProcessPtr &p);

		// Constructor / destructor
		explicit ProcessChain(	const string &name,
								ProcessRunMode runMode = Process_Run_Frame,
								ProcessQueueMode queueMode = Process_Queue_Multiple) :
			Process(name, runMode, queueMode)
		{}
		virtual ~ProcessChain() {}
};


/*=============================================================================
class ProcessGroup
	A process that contains a list of subprocesses (incl. chains and groups)
	where all must be finished before this process will be finished.
=============================================================================*/
class ProcessGroup : public Process {
	protected:
		ProcessList	m_process;

		/*---------------------------------------------------------------------
			A derived class must explicitly call this base class method, or
			implement the same functionality on its own.
		---------------------------------------------------------------------*/
		virtual void onUpdate(double deltaMillis);
		
		// hooks for this parent process to update member data based on subprocess results
		virtual void beforeOneUpdate(const ProcessPtr &p) {}
		virtual void afterOneUpdate(const ProcessPtr &p) {}
		virtual void afterOneFinish(const ProcessPtr &p) {}

	public:
		// Accessors
		inline size_t size() const;
		// Mutators
		inline size_t addProcess(const ProcessPtr &p);

		// Constructor / destructor
		explicit ProcessGroup(	const string &name,
								ProcessRunMode runMode = Process_Run_Frame,
								ProcessQueueMode queueMode = Process_Queue_Multiple) :
			Process(name, runMode, queueMode)
		{}
		virtual ~ProcessGroup() {}
};

#include "Impl/Process.inl"