/* TextWriter.h
Author: Jeff Kiah
Orig.Date: 06/09/2012
*/
#pragma once

template <class Implementation>
class TextWriter_Base {
	protected:

	public:

		explicit TextWriter_Base() {}
		virtual ~TextWriter_Base() {}
};

#if defined(WIN32)
#include "Impl/TextWriter_FW1.h"
#endif