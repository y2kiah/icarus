/* Font_GDI.cpp
Author: Jeff Kiah
Orig.Date: 06/09/2012
*/
#include "Font_GDI.h"
#include "Math/IMath.h"
#include <sstream>
#include "Render/Texture2D.h"
#include "Render/ImageProcessing.h"

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
bool Font_TTF::loadTrueTypeFont(uint32_t pointSize)
{
	// Do some debug-only error checking
	_ASSERTE(mFontName.length() < 32 && "lpszFace must not exceed 32 characters including terminating null");
	_ASSERTE(mFontWeight >= 0 && mFontWeight <= 1000 && "Weight out of range");

	mPointSize = pointSize;

	// Create a memory DC for the font
	mhFontDC = CreateCompatibleDC(NULL);
	if (!mhFontDC) {
		debugPrintf("Font_TTF: Error: CreateCompatibleDC() failed!\n");
		return false;
	}
	mInitFlags[INIT_DC] = true; // set the init flag
	
	// Calculate the required height for a certain point size
	//  uses number of pixels per logical inch along screen height
	int reqHeight = -((int)pointSize * GetDeviceCaps(mhFontDC, LOGPIXELSY)) / 72;

	mhFont = CreateFont(
				reqHeight,						// Height of Font
				0,								// Width of Font
				0,								// Angle of Escapement
				0,								// Orientation Angle
				mFontWeight,					// Font Weight
				(mItalic ? TRUE : FALSE),		// Italic
				(mUnderline ? TRUE : FALSE),	// Underline
				(mStrikeout ? TRUE : FALSE),	// Strikeout
				ANSI_CHARSET,					// Character Set Identifier
				OUT_TT_ONLY_PRECIS,				// Output Precision
				CLIP_DEFAULT_PRECIS,			// Clipping Precision
				(mAntialiased ?					// Output Quality
					ANTIALIASED_QUALITY :
					NONANTIALIASED_QUALITY),
				FF_DONTCARE | DEFAULT_PITCH,	// Family And Pitch
				mFontName.c_str());				// Font Face Name

	if (!mhFont) {
		debugPrintf("Font_TTF: Error: CreateFont() failed!\n");
		return false;
	}
	mInitFlags[INIT_FONT] = true; // set the init flag

	// Store the original default font per docs here
	//  http://msdn.microsoft.com/en-us/library/dd162957(VS.85).aspx
	mhFontOld = (HFONT)SelectObject(mhFontDC, mhFont);
	
	// Get font's character ABC widths to find out how far
	// to move the cursor horizontally after each character
	ABC abc[NUM_CHARS];
	if (!GetCharABCWidths(mhFontDC, FIRST_CHAR, LAST_CHAR, abc)) {
		debugPrintf("Font_TTF: Error: CreateFont() failed!\n");
		return false;
	}
	// Record the character widths and cursor offsets for font rendering
	for (int c = 0; c < NUM_CHARS; ++c) {
		mCharMetrics[c].width = abc[c].abcB + max(abc[c].abcA, 0) + max(abc[c].abcC, 0);
		mCharMetrics[c].preAdvance = min(abc[c].abcA, 0);
		mCharMetrics[c].postAdvance = abc[c].abcB + abc[c].abcC;
	}

	// Get the font's character height among other things
	if (!GetTextMetrics(mhFontDC, &mTextMetrics)) {
		debugPrintf("Font_TTF: Error: CreateFont() failed!\n");
		return false;
	}
	mCharHeight = mTextMetrics.tmHeight;

	return true;
}

/*---------------------------------------------------------------------
	Renders the glyphs to a bitmap in memory. If pointSize is 0, the
	bitmap size will be set by textureSize (which must be pwr 2) and
	the largest font size that fits within the texture will be
	determined automatically. If pointSize is > 0, textureSize will be
	ignored and the smallest pwr 2 size that fits the font size will be
	determined automatically. The specified number of pixels in gap
	will border each glyph.
---------------------------------------------------------------------*/
bool Font_TTF::renderToBitmap(uint32_t textureSize, uint32_t gap)
{
	// Do some input error checking
	if (!IMath::isPow2(textureSize)) {
		debugPrintf("Font_TTF: Error: textureSize %i not pwr 2 in renderToBitmap\n", textureSize);
		return false;
	}
	if (textureSize < 32 || textureSize > 4096) {
		debugPrintf("Font_TTF: Error: textureSize %i out of range (32-4096) in renderToBitmap\n", textureSize);
		return false;
	}

	// Create a memory bitmap for rendering glyphs to
	BITMAPINFO bmpInfo;
	SecureZeroMemory((PVOID)&bmpInfo, sizeof(bmpInfo));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = textureSize;
	bmpInfo.bmiHeader.biHeight = -(int)textureSize;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	mhBitmap = CreateDIBSection(mhFontDC, &bmpInfo, DIB_RGB_COLORS, (void**)&mpBits, NULL, 0);
	if (!mhBitmap) {
		debugPrintf("Font_TTF: Error: CreateDIBSection() failed!\n");
		return false;
	}
	mInitFlags[INIT_BITMAP] = true; // set the init flag

	// Select everything into the DC
	mhBitmapOld = (HBITMAP)SelectObject(mhFontDC, mhBitmap);
	SetBkColor(mhFontDC, RGB(0, 0, 0));			// make these colors selectable
	SetTextColor(mhFontDC, RGB(255, 255, 255));
	SetBkMode(mhFontDC, OPAQUE); // fill character background

	// Draw all characters
	uint32_t top = gap;
	uint32_t left = gap;
	for (int i = 0; i < NUM_CHARS; ++i) {
		// Start new row if needed
		if (left + mCharMetrics[i].width + gap > textureSize) {
			left = gap;
			top += mCharHeight + gap;
			if (top + mCharHeight + gap >= textureSize) {
				debugPrintf("Font_TTF: Error: Texture %ix%i too small for %s %i!\n", textureSize, textureSize, mFontName.c_str(), mPointSize);
				break;
			}
		}
		// Draw the glyph
		uint8_t c = (uint8_t)(i + FIRST_CHAR);
		if (!TextOutA(mhFontDC, left - mCharMetrics[i].preAdvance, top, (const char *)&c, 1)) {
			debugPrintf("Font_TTF: TextOut error on '%c'\n", (char)c);
		}
		left += mCharMetrics[i].width + gap;
	}

	debugPrintf("Font_TTF: maxWidth=%u   height=%u\n", mTextMetrics.tmMaxCharWidth, mCharHeight); // TEMP

	// look here for code:	http://members.gamedev.net/EvilSteve/ThreadStuff2/PFont.cpp
	// and:					http://members.gamedev.net/EvilSteve/ThreadStuff2/PFont.h
	// and:					http://www.gamedev.net/community/forums/topic.asp?topic_id=538837
	// Code to render the bitmap:	http://www.geocities.com/foetsch/bmpfonts/bmpfonts.htm

	return true;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
bool Font_TTF::saveGlyphsToDDSFile(uint32_t textureSize, uint32_t gap, const wstring &filename)
{
	// render the font to the in-memory bitmap first
	if (!mhBitmap) {
		if (!renderToBitmap(textureSize, gap)) {
			return false;
		}
	}

	// create glyphs texture
	Texture2D_D3D11 tex;
	Texture2DCreateDesc desc;
	desc.srcData = mpBits;
	desc.size = sizeof(mpBits);
	desc.srcChannels = 3;
	desc.srcWidth = desc.srcHeight = textureSize;
	desc.hasMipLevels = 1;
	desc.compress = desc.genMipmapChain = false;

	tex.createTexture(desc, mFontName);
	if (tex.initialized()) {
		wstring _filename(filename);
		if (_filename.empty()) {
			std::wostringstream ss;
			ss << mFontName << mPointSize << ".dds";
			_filename.assign(ss.str());
		}
		if (!tex.saveTextureToDDSFile(_filename)) {
			debugPrintf("Font_TTF: saveTextureToDDSFile failed!\n");
			return false;
		}
	} else {
		debugPrintf("Font_TTF: createTexture failed!\n");
		return false;
	}
	return true;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
bool Font_TTF::saveSDFToDDSFile(uint32_t textureSize, uint32_t gap, const wstring &filename)
{
	// render the font to the in-memory bitmap first
	if (!mhBitmap) {
		if (!renderToBitmap(textureSize, gap)) {
			return false;
		}
	}

	// create signed distance field font texture
	ImageBufferPtr sdfImgPtr(new ImageBuffer(textureSize, textureSize, 3, textureSize*3, ImageBuffer::Img_Type_RGB, (uint8_t*)mpBits));
	ImageBufferPtr newImgPtr = SDFFilter::process(*sdfImgPtr);
	ImageBuffer *newImg = static_cast<ImageBuffer*>(newImgPtr.get());

	// save sdf texture to file
	Texture2D_D3D11 tex;
	Texture2DCreateDesc desc;
	desc.srcData = newImg->pBits();
	desc.size = sizeof(newImg->pBits());
	desc.srcChannels = newImg->Bpp();
	desc.srcWidth = newImg->width();
	desc.srcHeight = newImg->height();
	desc.hasMipLevels = 1;
	desc.compress = desc.genMipmapChain = false;

	tex.createTexture(desc, L"sdf");
	if (tex.initialized()) {
		wstring _filename(filename);
		if (_filename.empty()) {
			std::wostringstream ss;
			ss << "sdf_" << mFontName << mPointSize << ".dds";
			_filename.assign(ss.str());
		}
		if (!tex.saveTextureToDDSFile(_filename)) {
			debugPrintf("Font_TTF: saveSDFTextureToDDSFile failed!\n");
			return false;
		}
	} else {
		debugPrintf("Font_TTF: createTexture failed!\n");
		return false;
	}
	return true;
}

/*---------------------------------------------------------------------
	Picks the smallest power of 2 size for a square texture that will
	fit all ANSI characters of the font. Returns 0 if no texture size
	in the range 32-4096 will fit the font.
---------------------------------------------------------------------*/
uint32_t Font_TTF::predictTextureSize(uint32_t pointSize, uint32_t gap) const
{
	// Do some debug-only error checking
	_ASSERTE(mFontName.length() < 32 && "lpszFace must not exceed 32 characters including terminating null");
	_ASSERTE(mFontWeight >= 0 && mFontWeight <= 1000 && "Weight out of range");

	// Create a temporary memory DC for the temp. font
	HDC tmpFontDC = CreateCompatibleDC(NULL);
	if (!tmpFontDC) { return 0; }

	int reqHeight = -((int)pointSize * GetDeviceCaps(tmpFontDC, LOGPIXELSY)) / 72;
	// load a temporary font to check for sizing
	HFONT tmpFont = CreateFont(
				reqHeight,						// Height of Font
				0,								// Width of Font
				0,								// Angle of Escapement
				0,								// Orientation Angle
				mFontWeight,					// Font Weight
				(mItalic ? TRUE : FALSE),		// Italic
				(mUnderline ? TRUE : FALSE),	// Underline
				(mStrikeout ? TRUE : FALSE),	// Strikeout
				ANSI_CHARSET,					// Character Set Identifier
				OUT_TT_ONLY_PRECIS,				// Output Precision
				CLIP_DEFAULT_PRECIS,			// Clipping Precision
				(mAntialiased ?					// Output Quality
					ANTIALIASED_QUALITY :
					NONANTIALIASED_QUALITY),
				FF_DONTCARE | DEFAULT_PITCH,	// Family And Pitch
				mFontName.c_str());				// Font Face Name
	if (!tmpFont) {
		DeleteDC(tmpFontDC);
		return 0;
	}
	HFONT tmpFontOld = (HFONT)SelectObject(tmpFontDC, tmpFont);
	// get text metrics
	BOOL result1 = FALSE, result2 = FALSE;
	ABC tmpAbc[NUM_CHARS];
	TEXTMETRIC tmpTextMetrics;
	result1 = GetCharABCWidths(tmpFontDC, FIRST_CHAR, LAST_CHAR, tmpAbc);
	result2 = GetTextMetrics(tmpFontDC, &tmpTextMetrics);
	// clean up
	SelectObject(tmpFontDC, tmpFontOld);
	DeleteObject(tmpFont);
	DeleteDC(tmpFontDC);
	if (result1 == FALSE || result2 == FALSE) { return 0; }

	// get the total volume of characters
	uint32_t volume = 0;
	for (int i = 0; i < NUM_CHARS; ++i) {
		uint32_t width = tmpAbc[i].abcB + max(tmpAbc[i].abcA, 0) + max(tmpAbc[i].abcC, 0);
		volume += (width + gap) * (tmpTextMetrics.tmHeight + gap);
	}
	// find the minimum possible size for the texture
	uint32_t size = 32;
	while (volume > (size*size)) {
		size <<= 1; // double the size
	}
	if (size > 4096) { return 0; } // 4096 is the largest size allowed

	// rendering might not actually fit on the minimum sized texture
	// perform rendering dry runs until the font fits for sure
	uint32_t left = gap, top = gap;
	bool finished = false;
	while (!finished) {
		finished = true;
		for (int i = 0; i < NUM_CHARS; ++i) {
			uint32_t width = tmpAbc[i].abcB + max(tmpAbc[i].abcA, 0) + max(tmpAbc[i].abcC, 0);
			if (left + width + gap >= size) {
				top += tmpTextMetrics.tmHeight + gap;
				if (top + tmpTextMetrics.tmHeight + gap >= size) {
					finished = false;
					break;
				}
				left = gap;
			}
			left += width + gap;
		}
		if (!finished) {
			size <<= 1; // double the size
			if (size > 4096) return 0; // 4096 is the largest size allowed
		}
	}
	return size;
}

/*---------------------------------------------------------------------
	Picks the largest point size that will fit onto a square pwr 2
	texture of the specified size. Returns 0 if no font size 8 or
	greater will fit on the texture.
---------------------------------------------------------------------*/
uint32_t Font_TTF::predictFontSize(uint32_t textureSize, uint32_t gap) const
{
	_ASSERTE(IMath::isPow2(textureSize) && "textureSize must be power of 2!");
	_ASSERTE(textureSize >= 32 && textureSize <= 4096 && "valid textureSize range from 32 to 4096!");

	// Brute force approach, increment font size until the max is found
	// I had to pick some upper limit, 1000 seems high enough that we won't run up on it for a 4096 texture
	// Arial size is 393 at 4096 texture size with 0 gap for an example
	uint32_t largestPointSize = 0;
	for (uint32_t pointSize = 8; pointSize <= 1000; ++pointSize) {
		uint32_t texSize = predictTextureSize(pointSize, gap);
		if (texSize == textureSize && pointSize > largestPointSize) {
			largestPointSize = pointSize;
		} else if (texSize == 0) {
			break;
		}
	}
	return largestPointSize;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
void Font_TTF::destroyFont()
{
	if (mInitFlags[INIT_BITMAP]) {
		SelectObject(mhFontDC, mhBitmapOld);
		DeleteObject(mhBitmap);
		mhBitmap = mhBitmapOld = NULL;
	}
	if (mInitFlags[INIT_FONT]) {
		SelectObject(mhFontDC, mhFontOld);
		DeleteObject(mhFont);
		mhFont = mhFontOld = NULL;
	}
	if (mInitFlags[INIT_DC]) {
		DeleteDC(mhFontDC);
		mhFontDC = NULL;
	}
}

Font_TTF::Font_TTF(const wstring &fontName, FontWeight weight,
				   bool italic, bool underline, bool strikeout,
				   bool antialiased) :
	mhFont(NULL), mhFontOld(NULL),
	mhBitmap(NULL), mhBitmapOld(NULL),
	mhFontDC(NULL),
	mCharHeight(0), mPointSize(0),
	mpBits(NULL),
	mFontName(fontName), mFontWeight(weight),
	mItalic(italic), mUnderline(underline), mStrikeout(strikeout),
	mAntialiased(antialiased),
	mInitFlags(0)
{
}

Font_TTF::~Font_TTF()
{
	destroyFont();
}