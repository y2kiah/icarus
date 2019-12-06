/* Font_GDI.h
Author: Jeff Kiah
Orig.Date:	06/08/2012
*/
#pragma once

#include "Application/Win32/Win32.h"
#include <cstdint>
#include <bitset>
#include <string>

using std::bitset;
using std::wstring;

///// STRUCTURES /////

class Font_TTF {
	public:
		///// DEFINITIONS /////
		enum : int {
			FIRST_CHAR = 32,
			LAST_CHAR = 126,
			NUM_CHARS = LAST_CHAR-FIRST_CHAR+1
		};
		enum FontWeight : int {
			Wt_DontCare = 0,
			Wt_Thin = 100,
			Wt_Normal = 400,
			Wt_Bold = 700,
			Wt_Heavy = 900
		};
		struct CharMetrics {
			uint32_t width;			// the total width of the character [ = b + max(abcA,0) + max(abcC,0) ]
			uint32_t preAdvance;	// the horizontal distance to move the cursor before rendering [ = min(abcA,0) ]
			uint32_t postAdvance;	// the horizontal distance to move the cursor after rendering [ = abcB + abcC ]
			uint32_t posX, posY;	// the upper-left x,y coordinate of the character on the bitmap
		};

	private:
		///// VARIABLES /////
		// initialization flags
		enum FontInitFlags {
			INIT_DC = 0,
			INIT_FONT,
			INIT_BITMAP,
			INIT_MAX
		};
		bitset<INIT_MAX>	mInitFlags;

	protected:
		HFONT			mhFont;
		HFONT			mhFontOld;
		HBITMAP			mhBitmap;
		HBITMAP			mhBitmapOld;
		HDC				mhFontDC;
		uint32_t		mCharHeight;
		uint32_t		mPointSize;
		CharMetrics		mCharMetrics[NUM_CHARS];
		TEXTMETRIC		mTextMetrics;
		DWORD *			mpBits;
		wstring			mFontName;
		FontWeight		mFontWeight;
		bool			mItalic, mUnderline, mStrikeout;
		bool			mAntialiased;

		/*---------------------------------------------------------------------
			Renders the glyphs to a bitmap in memory. If pointSize is 0, the
			bitmap size will be set by textureSize (which must be pwr 2) and
			the largest font size that fits within the texture will be
			determined automatically. If pointSize is > 0, textureSize will be
			ignored and the smallest pwr 2 size that fits the font size will be
			determined automatically. The specified number of pixels in gap
			will border each glyph.
		---------------------------------------------------------------------*/
		bool renderToBitmap(uint32_t textureSize, uint32_t gap);

	public:
		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Picks the smallest power of 2 size for a square texture that will
			fit all ANSI characters of the font. Returns 0 if no texture size
			in the range 32-4096 will fit the font.
		---------------------------------------------------------------------*/
		uint32_t predictTextureSize(uint32_t pointSize, uint32_t gap) const;

		/*---------------------------------------------------------------------
			Picks the largest point size that will fit onto a square pwr 2
			texture of the specified size. Returns 0 if no font size 8 or
			greater will fit on the texture.
		---------------------------------------------------------------------*/
		uint32_t predictFontSize(uint32_t textureSize, uint32_t gap) const;

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		bool loadTrueTypeFont(uint32_t pointSize);
				
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		bool saveGlyphsToDDSFile(uint32_t textureSize, uint32_t gap = 0, const wstring &filename = L"");
		
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		bool saveSDFToDDSFile(uint32_t textureSize, uint32_t gap = 0, const wstring &filename = L"");

		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		void destroyFont();

		// Constructor / destructor
		explicit Font_TTF(const wstring &fontName, FontWeight weight,
						  bool italic = false, bool underline = false, bool strikeout = false,
						  bool antialiased = true);
		~Font_TTF();
};