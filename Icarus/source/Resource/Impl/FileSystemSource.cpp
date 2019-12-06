/* FileSystemSource.cpp
Author: Jeff Kiah
Orig.Date: 07/09/2012
*/
#include "Resource/FileSystemSource.h"
#include <cstdlib>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

bool FileSystemSource::directoryExists(const wstring &relativePath)
{
	wchar_t fullPath[1024] = L"\0";
	_wfullpath(fullPath, relativePath.c_str(), 1024);
	
	if (_waccess(fullPath, 0) == 0) {
        struct _stat status;
        _wstat(fullPath, &status);

        return (status.st_mode & S_IFDIR) != 0;
    }
    return false;
}

/*---------------------------------------------------------------------
	Files are opened in the getResource method. This implementation
	returns true if the path exists, false if not.
---------------------------------------------------------------------*/
bool FileSystemSource::open()
{
	return directoryExists(m_rootPath);
}

size_t FileSystemSource::getResourceSize(const wstring &resName) const
{
	const wstring resPath(m_rootPath + resName);

	FILE *inFile = _wfsopen(resPath.c_str(), L"rb", _SH_DENYWR);
	if (!inFile) {
		debugWPrintf(L"FileSystemSource: file %s not found\n", resName.c_str());
		return 0;
	}
	// get size
	fseek(inFile, 0, SEEK_END);
	size_t size = ftell(inFile);
	fclose(inFile);
	return size;
}

size_t FileSystemSource::getResource(const wstring &resName, CharBufferPtr &dataPtr, size_t threadIndex)
{
	const wstring resPath(m_rootPath + resName);

	FILE *inFile = _wfsopen(resPath.c_str(), L"rb", _SH_DENYWR);
	if (!inFile) {
		debugWPrintf(L"FileSystemSource: file %s not found\n", resName.c_str());
		return 0;
	}
	// get size
	fseek(inFile, 0, SEEK_END);
	size_t size = ftell(inFile);
	// read data from file
	rewind(inFile);
	CharBufferPtr bPtr(new char[size], checked_array_deleter<char>());
	dataPtr = bPtr;
	void *buffer = static_cast<void *>(dataPtr.get());
	size_t sizeRead = fread(buffer, 1, size, inFile);
	fclose(inFile);
	if (sizeRead != size) {
		debugWPrintf(L"FileSystemSource: file %s read error\n", resName.c_str());
		return 0;
	}
	return size;
}

/*---------------------------------------------------------------------
	Returns 0 for all threads, file operations are already thread safe.
---------------------------------------------------------------------*/
size_t FileSystemSource::getNewThreadIndex()
{
	return 0;
}