/* Renderer.h
Author: Jeff Kiah
Orig.Date: 5/22/12
Description: Renderer is a base class to be implemented using static
	polymorphism and the curiously recurring template pattern (CRTP).
	http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
	A #define can be used choose a specific implementation for at
	compile time. If it is required to support multiple renderers on
	the same platform (and within the same exe), a runtime decision
	can be made on which implementation to construct and passing the
	corresponding class in the template parameter.
*/
#pragma once

class Platform;

template <class Implementation>
class Renderer_Base {
	protected:
		///// VARIABLES /////
		const Platform *m_pPlatform;

		///// FUNCTIONS /////

		// This class can't be instantiated directly
		explicit Renderer_Base(const Platform *pPlatform) :
			m_pPlatform(pPlatform)
		{}

	public:
		bool initRenderer() {
			return static_cast<Implementation*>(this)->initRenderer();
		}

		void cleanup() {
			static_cast<Implementation*>(this)->cleanup();
		}

		void render() {
			static_cast<Implementation*>(this)->render();
		}

		virtual ~Renderer_Base() { cleanup(); }
};

#if defined(WIN32)
#include "Impl/Renderer_d3d11.h"
#endif