/* RenderBuffer_d3d11.cpp
Author: Jeff Kiah
Orig.Date: 06/03/2012
*/

#include "RenderBuffer_d3d11.h"
#include "Render/Vertex_Defs.h"

#if defined(WIN32)

///// STATIC VARIABLES /////

RendererWeakPtr RenderBuffer_D3D11::sRenderer;

///// FUNCTIONS /////

bool RenderBuffer_D3D11::createFromMemory(BufferType type, const void *pData, uint32_t size,
										  uint32_t stride, uint32_t offset)
{
	// get the D3D11Device
	ID3D11DevicePtr d3dDevice = getD3D11DevicePtr();
	if (!d3dDevice) { return false; }

	D3D11_BUFFER_DESC bd;
	bd.Usage = (type == ConstantBuffer) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = type; // types are equivalent to D3D11_BIND_VERTEX_BUFFER, D3D11_BIND_INDEX_BUFFER, D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = (type == ConstantBuffer) ? D3D11_CPU_ACCESS_WRITE : 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA id;
	id.pSysMem = pData;
	id.SysMemPitch = 0;
	id.SysMemSlicePitch = 0;

	HRESULT hr = d3dDevice->CreateBuffer(&bd, &id, &m_buffer.GetInterfacePtr());
	if (FAILED(hr)) {
		return false;
	}

	m_stride = stride;
	m_offset = offset;

	return true;
}

/*---------------------------------------------------------------------
	returns the device from Engine::instance().getRenderer()
---------------------------------------------------------------------*/
ID3D11DevicePtr RenderBuffer_D3D11::getD3D11DevicePtr()
{
	// get Renderer from the Engine singleton instance
	RendererImpl *pRend = static_cast<RendererImpl *>(sRenderer.lock().get());
	if (!pRend) {
		_ASSERTE(false && "Attempted to get null Renderer pointer");
		return ID3D11DevicePtr();
	}

	// get the D3D11Device
	if (!pRend->getDevice()) {
		_ASSERTE(false && "Attempted to get null D3D11Device pointer");
		return ID3D11DevicePtr();
	}

	return pRend->getDevice();
}

#endif