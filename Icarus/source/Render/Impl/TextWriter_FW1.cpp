/* TextWriter_FW1.cpp
Author: Jeff Kiah
Orig.Date: 06/09/2012
*/
#include "Render/TextWriter.h"

#if defined(WIN32)

#if defined(_DEBUG)
#pragma comment( lib, "FW1FontWrapper_d.lib" )
#else
#pragma comment( lib, "FW1FontWrapper.lib" )
#endif

bool TextWriter_FW1::init(const wstring &fontFamily, const ID3D11DevicePtr &pDevice)
{
	IFW1FactoryPtr factoryPtr;
	IFW1Factory *pFactory = 0;
	HRESULT hr = FW1CreateFactory(FW1_VERSION, &pFactory);
	if (FAILED(hr)) {
		return false;
	}
	factoryPtr.Attach(pFactory, false);
	
	IFW1FontWrapper *pWrapper = 0;
	hr = pFactory->CreateFontWrapper(pDevice.GetInterfacePtr(),
									 fontFamily.c_str(),
									 &pWrapper);
	if (FAILED(hr)) {
		return false;
	}
	m_fontWrapper.Attach(pWrapper, false);

	m_initialized = true;

	return true;
}

void TextWriter_FW1::drawText(const wstring &text, float fontSize, float posX, float posY,
							  uint32_t textColor, const ID3D11DeviceContextPtr &pContext)
{
	if (!initialized()) { return; }

	m_fontWrapper->DrawString(
		pContext.GetInterfacePtr(),
		text.c_str(),
		fontSize,			// Font size
		posX,				// X position
		posY,				// Y position
		textColor,			// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE	// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
		//| FW1_CENTER | FW1_VCENTER
	);
}

#endif