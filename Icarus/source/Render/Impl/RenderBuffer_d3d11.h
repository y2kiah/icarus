/* RenderBuffer_d3d11.h
Author: Jeff Kiah
Orig.Date: 06/03/2012
*/
#pragma once
#if defined(WIN32)

#include "Render/RenderBuffer.h"
#include "Renderer_d3d11.h"

class RenderBuffer_D3D11 :
	public RenderBuffer_Base<RenderBuffer_D3D11>
{
	friend class Renderer_D3D11;

	private:
		///// VARIABLES /////
		// Static
		static RendererWeakPtr sRenderer;

		// Member
		ID3D11BufferPtr	m_buffer;

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			returns the device from renderer
		---------------------------------------------------------------------*/
		static ID3D11DevicePtr getD3D11DevicePtr();

	public:
		// Accessors
		const ID3D11BufferPtr &getBuffer() const { return m_buffer; }

		// Mutators
		bool createFromMemory(BufferType type, const void *pData, uint32_t size,
							  uint32_t stride, uint32_t offset);

		// Constructor / destructor
		explicit RenderBuffer_D3D11() :
			RenderBuffer_Base()
		{}

		virtual ~RenderBuffer_D3D11() {}
};

typedef RenderBuffer_Base<RenderBuffer_D3D11>	RenderBuffer;
typedef RenderBuffer_D3D11						RenderBufferImpl;

typedef unique_ptr<RenderBuffer>		RenderBufferUniquePtr;
typedef vector<RenderBufferUniquePtr>	RenderBufferPtrList;

#endif