/*----==== ZIPFILE.H ====----
	Author:		Jeff Kiah
	Orig.Date	05/25/2009
	Rev.Date	06/10/2009
	Purpose:	The declaration of a quick'n dirty ZIP file reader class. Original code from Javier Arevalo.
				zlib at http://www.cdrom.com/pub/infozip/zlib/
				Got code from Game Coding Complete 3rd Edition and modified it for this engine.
---------------------------*/

#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <hash_map>
#include <boost/optional.hpp>
#include "ResCache.h"

using std::string;
using std::wstring;
using std::vector;
using std::hash_map;
using boost::optional;

typedef hash_map<wstring, int>	ZipContentsMap;		// maps path to a zip content id

/*=============================================================================
class ZipFile
	Resource Caching System:
		ZipFile has been enhanced to work with the resource caching system.
	This class is-a IResourceSource in the resource caching system. The
	original public interface has been made private and used internally. The
	new public interface (as defined by the base interface) is more user
	friendly and doesn't require the client to have knowledge of the inner
	workings (such as converting filenames into offsets, etc.).
	Thread Safety:
		ZipFile has been made thread-safe by the use of unique file pointers,
	1 per thread. Each thread that would like to read from the file should
	first request and store a new index via getNewThreadIndex(). The index is
	then used exclusively by that thread when calling getResource(). Index 0 is
	the default and always exists, more often than not reserved for use by the
	"main" thread.
=============================================================================*/
class ZipFile : public IResourceSource {
	private:
		///// DECLARATIONS /////
		struct	TZipLocalHeader;
		struct	TZipDirHeader;
		class	TZipDirFileHeader;

		///// VARIABLES /////
		vector<FILE*>	mFile;	// zip file pointers, one per thread that needs to read from the file
		char *	mDirData;		// raw data buffer
		int		mEntries;		// number of entries

		const TZipDirFileHeader	**mDirHdr;	// pointers to the dir entries in mDirData
		
		wstring	mZipFilename;	// filename of the archive

		// initialization flags
		enum EffectInitFlags {
			INIT_OPEN = 0,
			INIT_MAX
		};
		bitset<INIT_MAX>	mInitFlags;

		///// FUNCTIONS /////
		void	getFilename(int i, char *pszDest) const;
		size_t	getFileLen(int i) const;
		bool	readFile(int i, void *pBuf, size_t threadIndex);
		bool	readLargeFile(int i, void *pBuf, void (*callback)(int, bool &), int threadIndex);
		
		optional<int> find(const wstring &path) const;
		void	close();

	public:
		///// VARIABLES /////
		ZipContentsMap	mZipContentsMap;

		///// FUNCTIONS /////
		// Interface functions
		virtual bool	open();
		virtual size_t	getResourceSize(const wstring &resName) const;
		virtual size_t	getResource(const wstring &resName, CharBufferPtr &dataPtr, size_t threadIndex = 0);

		/*---------------------------------------------------------------------
			Creates a new file pointer and returns the index, or -1 on error.
		---------------------------------------------------------------------*/
		virtual size_t	getNewThreadIndex();

		// Accessors
		int				getNumFiles() const		{ return mEntries; }
		const wstring &	getZipFilename() const	{ return mZipFilename; }

		// Constructor / destructor
		explicit ZipFile(const wstring &zipFilename) :
			mZipFilename(zipFilename),
			mEntries(0), mDirData(0), mInitFlags(0)
		{
			mFile.reserve(2);	// reserve capacity for 2 threads
			mFile.push_back(0); // create one pointer for the main (default) thread
		}
		~ZipFile() {
			close();
		}
};


