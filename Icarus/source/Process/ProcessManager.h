/* ProcessManager.h
Author: Jeff Kiah
Orig.Date: 05/30/2012
*/
#pragma once

#include "Process.h"
#include "Utility/Debug.h"

///// STRUCTURES /////

/*=============================================================================
class ProcessManager
=============================================================================*/
class ProcessManager {
	private:
		///// VARIABLES /////
		ProcessLinkedList		mProcessList;

		// also need a multimap for random searches for processes and detaching

		///// FUNCTIONS /////
		void	detach(const ProcessPtr &procPtr);

	public:
		bool	isProcessActive(const string &procName);
		bool	hasProcesses() const	{ return !mProcessList.empty(); }

		void	attach(const ProcessPtr &procPtr);

		void	updateProcesses(double deltaMillis);

		/*---------------------------------------------------------------------
			destroys all processes in the list
		---------------------------------------------------------------------*/
		void	clear();

		explicit ProcessManager() {}
		~ProcessManager() {}
};