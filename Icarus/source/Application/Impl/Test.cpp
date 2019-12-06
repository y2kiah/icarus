/* Test.cpp
Author: Jeff Kiah
Orig.Date: 06/30/2012
*/

#include "Application/Test.h"

// testing
#include "Resource/ZipFile.h"
#include "Render/Texture2D.h"
#include "Render/RenderBuffer.h"
#include "Render/Vertex_Defs.h"
#include "Render/Mesh.h"
#include "Render/Effect.h"

/*#include "Math/NoiseWrapper.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/TVector4.h"
#include "Utility/Quadtree.h"
#include "Scripting/ScriptingEvents.h"
#include "Resource/ResHandle.h"
#include "UI/UISkin.h"
#include "Render/RenderObject_D3D9.h"

#include "Physics/FlightModel.h"
#include "Physics/Airfoil.h"
#include "EngineEvents.h"
*/

void TestProcess::onUpdate(double deltaMillis)
{
	// don't have the resource yet, so try to get it
	if (!mTexture.get() && !mTextureError) {
		ResHandle h;
		ResLoadResult retVal = h.tryLoad<Texture2DImpl>(L"textures/dirtnew.dds");
		if (retVal == ResLoadResult_Success) {
			mTexture = h.mResPtr;
			Texture2D *tex = static_cast<Texture2D*>(mTexture.get());
			debugWPrintf(L"SUCCESS loaded %s\n", tex->name().c_str());
		} else if (retVal == ResLoadResult_Error) {
			mTextureError = true;
			debugPrintf("ERROR loading dirtnew.dds\n");
		}
	}

	// test resource injection method
	if (!mTexture2TestDone) {
		mTexture2.reset(new Texture2DImpl());

		Texture2D *tex = static_cast<Texture2D*>(mTexture2.get());
		if (tex->loadFromFile(L"data/textures/palmtr.dds")) {
			if (!Texture2D::injectIntoCache(mTexture2, ResCache_Texture)) {
				debugPrintf("ERROR injecting palmtr.dds\n");
			}
		}
		mTexture2TestDone = true;
	} else if (mTexture2) {
		mTexture2->removeFromCache();
		mTexture2 = 0;
	}

	if (mTexture && !mTextureSaved) {
		Texture2D_D3D11 *pTex = static_cast<Texture2D_D3D11 *>(mTexture.get());
		pTex->saveTextureToDDSFile(L"data/dirtnew_saved.dds"); // test dds save
		mTextureSaved = true;
	}

/*			mMillis += deltaMillis;
	while (mMillis >= 1000) { // every second, print to the console
		debugPrintf("%s: millisecond count %0.0f\n", name().c_str(), mTotalMillis);
		mMillis -= 1000.0f;
	}

	mTotalMillis += deltaMillis;
	if (mTotalMillis >= 4000.0f) finish(); // run for 4 seconds
*/

	// Load the effect that was just compiled through resource system
	if (!mEffect.get() && !mEffectError) {
		ResHandle h;
		ResLoadResult retVal = h.tryLoad<EffectImpl>(L"data/shaders/Default.ifx");
		if (retVal == ResLoadResult_Success) {
			mEffect = h.mResPtr;
			Effect *eff = static_cast<Effect*>(mEffect.get());
			debugWPrintf(L"SUCCESS loaded %s\n", eff->name().c_str());
		} else if (retVal == ResLoadResult_Error) {
			mEffectError = true;
			debugPrintf("ERROR loading Default.ifx\n");
		}
	}

	// get some info on effect file renderable
//			Effect_D3D9 *pEff = static_cast<Effect_D3D9*>(mEffectFile.get());
//			if (pEff) {
//				debugPrintf("EFFECT FILE RENDERABLE: %i\n", pEff->isReadyForRender());
//			}

	///// TEST FONT_TTF /////
//			if (!mFontTested) {
//				Font_TTF font(L"Courier New", Font_TTF::Wt_Normal, false, false, false, false);
//				uint32_t texSize = 4096;
//				uint32_t gap = 0;
//				int pointSize = font.predictFontSize(texSize, gap);
//				font.loadTrueTypeFont(pointSize);
//				font.saveGlyphsToDDSFile(texSize, gap, L"data/textures/CourierNew_glyphs.dds");
//				font.saveSDFToDDSFile(texSize, gap, L"data/textures/CourierNew_sdf.dds");
//				debugPrintf("Font rendered %ix%i at %i pt gap %i\n", texSize, texSize, pointSize, gap);
//				mFontTested = true;
//			}
//			/////////////////////////

	///// TEST UISKIN /////
//			ResHandle hUISkin;
//			if (!hUISkin.load<UI::UISkin>("ui/test.xml")) {
		// handle error
//			}
//			UI::UISkin *pSkin = static_cast<UI::UISkin*>(hUISkin.getResPtr().get());
	///////////////////////

	///// TEST EVENT SYSTEM /////

	// test event trigger handlers
//			events.registerEventType("EVENT_SOMETHING", RegEventPtr(new ScriptCallableCodeEvent<EmptyEvent>(EventDataType_Empty)));
	//events.registerEventType("EVENT_SOMETHING", RegEventPtr(new ScriptCallableCodeEvent<EmptyEvent>(EventDataType_Empty)));
//			events.trigger("EVENT_SOMETHING");

	//events.trigger("EVENT_SPECIFIC");
	//EventPtr ePtr(new LuaFunctionEvent());
	//events.trigger(ePtr);
	// test event queue
	//events.raise("EVENT_SOMETHING");
	//events.notifyQueued(0);
	// get debug output
	//delete mEventMgr;
	//mEventMgr = new EventManager();

	/////////////////////////////
}

bool TestProcess::onInitialize()
{
	// Create the vertex buffer
	RenderBufferUniquePtr vBuf(new RenderBufferImpl());
	Vertex_P pData[] = {
		Vector3f(0.0f, 0.5f, 0.5f),
		Vector3f(0.5f, -0.5f, 0.5f),
		Vector3f(-0.5f, -0.5f, 0.5f)
	};
			
	if (!vBuf->createFromMemory(RenderBuffer::VertexBuffer,
								pData,
								sizeof(pData),
								sizeof(Vertex_P),
								0))
	{
		debugPrintf("ERROR: Create vertex buffer returned false\n");
	} else {
		debugPrintf("Vertex buffer created size=%u\n", sizeof(pData));
	}

	// Test Mesh
	if (mdl.importFromFile("data/models/palmtree/palmtree.x")) {
		debugWPrintf(L"Model %s imported\n", mdl.name().c_str());
	} else {
		debugWPrintf(L"ERROR: Model %s not imported\n", mdl.name().c_str());
	}

	// Test Effect
	EffectPtr ePtr(new EffectImpl());
	EffectImpl *pEffect = static_cast<EffectImpl *>(ePtr.get());
	if (pEffect->compileShadersToEffect(L"./source/Shaders/DefaultVS.hlsl",
										L"./source/Shaders/DefaultPS.hlsl",
										L"./data/shaders/Default.ifx"))
	{
		debugWPrintf(L"Effect compiled\n");
	} else {
		debugWPrintf(L"ERROR: Effect not compiled\n");
	}

	return true;
}

TestProcess::TestProcess(const string &name) :
	Process(name, Process_Run_Async, Process_Queue_Multiple),
	mTexture(), mTexture2(), mEffect(), mTextureError(false),
	mEffectError(false), mTextureSaved(false), mFontTested(false),
	mTexture2TestDone(false)
{}

TestProcess::~TestProcess()
{
	finish();
}
