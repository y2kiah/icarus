/*----==== IMAGEPROCESSING.H ====----
	Author:		Jeff Kiah
	Orig.Date:	07/17/2009
	Rev.Date:	07/17/2009
-----------------------------------*/

#pragma once

#include <cstdint>
#include <memory>
#include "Utility/Debug.h"

using std::shared_ptr;

///// DEFINITIONS /////
class ImageBuffer;
typedef shared_ptr<ImageBuffer>	ImageBufferPtr;

///// STRUCTURES /////

/*=============================================================================
=============================================================================*/
class Color {
	public:
		///// VARIABLES /////
		uint8_t r, g, b, a;

		///// FUNCTIONS /////
		void setRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
			r = red; g = green; b = blue; a = alpha;
		}
		///// OPERATORS /////
		void operator= (const Color &c) {
			r = c.r; g = c.g; b = c.b; a = c.a;
		}
		// Constructor / destructor
		Color() : r(0), g(0), b(0), a(0) {}
		Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : r(red), g(green), b(blue), a(alpha) {}
		Color(const Color &c) : r(c.r), g(c.g), b(c.b), a(c.a) {}
};

/*=============================================================================
class ImageBuffer
=============================================================================*/
class ImageBuffer {
	public:
		///// DEFINITIONS /////
		enum ImageType : int {
			Img_Type_A = 0,
			Img_Type_RGB,
			Img_Type_RGBA,
			Img_Type_ARGB,
			Img_Type_ABGR
		};

	private:
		///// VARIABLES /////
		uint8_t *		mpBits;			// pointer to pixel data
		uint32_t		mWidth;			// width in pixels
		uint32_t		mHeight;		// height in pixels
		uint32_t		mBpp;			// Bytes per pixel (RGBA would be 4)
		uint32_t		mPitch;			// this would normally be (width * Bpp), but could be another value
		ImageType		mImgType;		// format of the pixels stored in pBits
		bool			mOwnImageData;	// true if data has been allocated for bits, false if they
										// just point to data owned somewhere else
	public:
		///// FUNCTIONS /////
		// Accessors
		const uint8_t *	pBits() const	{ return mpBits; }
		uint32_t		width() const	{ return mWidth; }
		uint32_t		height() const	{ return mHeight; }
		uint32_t		Bpp() const		{ return mBpp; }
		uint32_t		pitch() const	{ return mPitch; }
		ImageType		imgType() const { return mImgType; }
		bool			hasOwnImageData() const { return mOwnImageData; }

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		bool getRGBA(uint32_t x, uint32_t y, Color &outColor) const;

		// Mutators
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		void setRGBA(uint32_t x, uint32_t y, const Color &c);

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		bool allocateBits(uint32_t width, uint32_t height, ImageType imgType);

		// Constructor / destructor
		explicit ImageBuffer() :
			mWidth(0), mHeight(0), mBpp(0), mPitch(0),
			mImgType(Img_Type_A), mpBits(0), mOwnImageData(false)
		{}
		explicit ImageBuffer(uint32_t width, uint32_t height, uint32_t Bpp, uint32_t pitch,
							 ImageType imgType, uint8_t *pBits) :
			mWidth(width), mHeight(height), mBpp(Bpp), mPitch(pitch),
			mImgType(imgType), mpBits(pBits), mOwnImageData(false)
		{}
		~ImageBuffer() {
			if (mOwnImageData && mpBits) { delete [] mpBits; }
		}
};

/*=============================================================================
class SDFFilter
	An image filter to create a low resolution signed distance field from a
	high resolution input image. The input image is treated as binary data
	(in/out) where any value > 0 is in, and 0 is out.
=============================================================================*/
class SDFFilter {
	private:
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		static float findSignedDistance(int pointX, int pointY, const ImageBuffer &inImage,
										int scanWidth, int scanHeight);

	public:
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		static ImageBufferPtr process(const ImageBuffer &inImage);
};