/* RenderBuffer.h
Author: Jeff Kiah
Orig.Date: 06/03/2012
*/
#pragma once

#include <cstdint>

template <class Implementation>
class RenderBuffer_Base
{
	public:
		///// DEFINITIONS /////
		enum BufferType {
			VertexBuffer = 0x1L,
			IndexBuffer = 0x2L,
			ConstantBuffer = 0x4L
		};

		///// FUNCTIONS /////
		// Accessors
		BufferType	type() const { return m_type; }
		uint32_t	stride() const { return m_stride; }
		uint32_t	offset() const { return m_offset; }

		// Mutators
		bool createFromMemory(BufferType type, const void *pData, uint32_t size,
							  uint32_t stride, uint32_t offset)
		{
			return static_cast<Implementation*>(this)->createFromMemory(type, pData, size, stride, offset);
		}

		virtual ~RenderBuffer_Base() {}

	protected:
		///// VARIABLES /////
		BufferType	m_type;
		uint32_t	m_stride;
		uint32_t	m_offset;

		// Constructor
		explicit RenderBuffer_Base() {}
};

#if defined(WIN32)
#include "Impl/RenderBuffer_d3d11.h"
#endif