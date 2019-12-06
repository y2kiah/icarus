/* TextWriter_FW1.h
Author: Jeff Kiah
Orig.Date: 06/09/2012
*/
#pragma once
#if defined(WIN32)

#include "Render/TextWriter.h"
#include "D3D11_Defs.h"

///// STRUCTURES /////

class TextWriter_FW1 : public TextWriter_Base<TextWriter_FW1>
{
	private:
		///// VARIABLES /////
		IFW1FontWrapperPtr	m_fontWrapper;
		bool	m_initialized;

	public:
		///// FUNCTIONS /////
		// Accessors
		bool initialized() const { return m_initialized; }
		
		// Mutators
		bool init(const wstring &fontFamily, const ID3D11DevicePtr &pDevice);
		void drawText(const wstring &text, float fontSize, float posX, float posY,
					  uint32_t textColor, const ID3D11DeviceContextPtr &pContext);
		
		// Constructor / destructor
		explicit TextWriter_FW1() :
			TextWriter_Base(), m_fontWrapper(0), m_initialized(false)
		{}

		virtual ~TextWriter_FW1() {}
};

typedef TextWriter_Base<TextWriter_FW1> TextWriter;
typedef TextWriter_FW1					TextWriterImpl;
typedef shared_ptr<TextWriter>			TextWriterPtr;

#endif