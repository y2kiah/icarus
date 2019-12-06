/* FileSystemSource.h
Author: Jeff Kiah
Orig.Date: 07/09/2012
*/
#pragma once

#include "ResCache.h"
#include <string>

using std::wstring;

/*=============================================================================
class FileSystemSource
	This source reads files directly from the file system. The path sets the
	source root. Use this resource source mainly for development.
=============================================================================*/
class FileSystemSource : public IResourceSource
{
	private:
		wstring m_rootPath;

		static bool directoryExists(const wstring &path);

	public:
		///// FUNCTIONS /////
		// Interface functions
		virtual bool	open();
		virtual size_t	getResourceSize(const wstring &resName) const;
		virtual size_t	getResource(const wstring &resName, CharBufferPtr &dataPtr, size_t threadIndex = 0);

		/*---------------------------------------------------------------------
			Creates a new file pointer and returns the index, or -1 on error.
		---------------------------------------------------------------------*/
		virtual size_t	getNewThreadIndex();

		// Constructor / destructor
		explicit FileSystemSource(const wstring &rootPath) :
			m_rootPath(rootPath)
		{}
};