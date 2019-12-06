/* Test.h
Author: Jeff Kiah
Orig.Date: 06/30/2012
*/
#pragma once

#include "Process/Process.h"
#include "Resource/ResHandle.h"
#include "Render/Mesh.h"

class TestProcess : public Process {
	private:
		ResPtr		mTexture;
		ResPtr		mTexture2;
		ResPtr		mEffect;
		bool		mTextureError;
		bool		mEffectError;
		bool		mTextureSaved;
		bool		mFontTested;
		bool		mTexture2TestDone;
		Model		mdl;

	public:
		virtual void onUpdate(double deltaMillis);

		virtual bool onInitialize();

		virtual void onFinish() {}
		virtual void onTogglePause() {}

		explicit TestProcess(const string &name);

		virtual ~TestProcess();
};