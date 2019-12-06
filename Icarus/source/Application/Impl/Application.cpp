/* Application.cpp
Author: Jeff Kiah
Orig.Date: 06/21/2012
*/

#include "Application/Application.h"
#include "Application/Timer.h"
#include "Event/EventManager.h"
#include "Event/RegisteredEvents.h"
#include "Process/ProcessManager.h"
#include "Resource/ResCache.h"
#include "Script/ScriptManager_LuaJIT.h"

// Temp
#include "Resource/ZipFile.h"
#include "Resource/FileSystemSource.h"
#include "Application/Test.h"

///// FUNCTIONS /////

void Application::processFrame()
{
	double updateDeltaMS = mFrameTimer->stop();
	mFrameTimer->start();
	if (!isPaused()) {
		update(updateDeltaMS);
	}

	// if screen refresh time has passed, render the output
//	if (mDeviceLost) {
		// handle device lost situation
//		handleLostDevice();
//	} else {
		// render the scene
//		mRenderTimer.stop();
//		if (mRenderTimer.millisecondsPassed() >= 16.66667f) { // should use a supplied refresh rate for this
//			mRenderTimer.start();
			render();
//		}
//	}
}

void Application::update(double deltaMillis)
{
	mEventMgr->notifyQueued(0);
	mScheduler->updateProcesses(deltaMillis);
}

void Application::render()
{
	mRenderer->render();

//	mRenderMgr->prepareSubmitList();

	// temp - use camera in scene graph, update will set the orientation, so just grab it here instead of setting it
//	activeCam->setViewTranslationYawPitchRoll(Vector3f(0.0f, 0.0f, -12000000.0f), Vector3f(0.0f, 0.0f, 0.0f));

//	if (!mRenderMgr->render(*activeCam)) {
//		pause();
//	}
	
//	mRenderMgr->resetSubmitList();
}

void Application::resetAfterInactive()
{
	// reset frame times so there isn't a big jump
	mFrameTimer->start();
}

///// TEST Mesh /////
//	testMesh = new Mesh_D3D9;
//	testMesh->loadFromXFile("data/model/palmtree.x");
///////////////////////

///// TEST NOISEWRAPPER /////
/*	HighPerfTimer *codeTimer = new HighPerfTimer();
	NoiseWrapper::setCoherentNoiseType(NOISE_TYPE_TEST);	
NoiseWrapper::initGrad4();
	const int texSizeX = 256;
	const int texSizeY = 256;
	const float length = 16.0f;
	const int numChannels = 3;

	uchar *buffer = new uchar[texSizeY*texSizeX*numChannels];
	
	const float xStep = length / static_cast<float>(texSizeX);
	const float yStep = length / static_cast<float>(texSizeY);

	codeTimer->start();
	float cx = 0, cy = 0;
	for (int y = 0; y < texSizeY; ++y) {
		for (int x = 0; x < texSizeX; ++x) {
			NoiseWrapper::setMultiFractalOperation(NOISE_MFO_ADD);
			//float nResult = NoiseWrapper::multiFractal(2, Vector2f(cx,cy).v, 1, 2.0f, 0.3f, 0.6f);
			float nResult = NoiseWrapper::fBm(2, Vector2f(cx,cy).v, 8, 2.0f, 0.5f, 1.0f);
			nResult += 1.0f; nResult *= 0.5f; // normalize to [0,1] range
			nResult *= 0.7f; nResult += 0.2f;
//			NoiseWrapper::setMultiFractalOperation(NOISE_MFO_ADD_ABS);
//			float nResult2 = NoiseWrapper::fBm(2, Vector2f(cx,cy).v, 18, 2.0f, 0.5f, 0.6f);
//			nResult2 *= nResult2;

//			nResult *= nResult2;		
			
			int iResult = math.f2iRound(nResult * 255);  // convert to int
			uchar ucResult = (iResult > 255) ? 255 : ((iResult < 0) ? 0 : iResult); // convert to uchar 

			int index = (y*texSizeX*numChannels) + (x*numChannels);
			for (int c = 0; c < numChannels; ++c) buffer[index+c] = ucResult;

			cx += xStep;
		}
		cx = 0;
		cy += yStep;
	}	
	codeTimer->stop();
	debugPrintf("ms to generate = %Lf\n", (double)codeTimer->secondsPassed() * 1000.0);
	
	ofstream fOut;
	fOut.open("time.txt", std::ios::out | std::ios::trunc);
	fOut << (double)codeTimer->secondsPassed() * 1000.0;
	fOut.close();

	Texture_D3D9 *d3dTexture = new Texture_D3D9();
	d3dTexture->createTexture(buffer, numChannels, texSizeX, texSizeY, TEX_FMT_UCHAR, TEX_USAGE_DEFAULT,
		TEX_RPT_WRAP, TEX_FILTER_DEFAULT, false, false, L"fBm");

	if (d3dTexture->isInitialized()) d3dTexture->saveTextureToFile("cached_quintic_8_oct.jpg");

	delete d3dTexture;
*/	
	// convert to single channel
/*	uchar *buffer2 = new uchar[texSizeY*texSizeX];
	int op = 0;
	for (int p = 0; p < texSizeY*texSizeX; ++p) {
		buffer2[p] = buffer[op];
		op += numChannels;
	}
	ofstream fOut;
	fOut.open("map_02.raw",std::ios::out | std::ios::trunc | std::ios::binary);
	fOut.write((const char *)buffer2, sizeof(uchar) * texSizeY * texSizeX);
	fOut.close();
	
	delete [] buffer2;*/
//	delete [] buffer;
//	delete codeTimer;

/////////////////////////////

//// TEST QUADTREE TRAVERSAL ////
/*	HighPerfTimer *codeTimer = new HighPerfTimer();
	Quadtree32 qt(4);

	codeTimer->start();
	qt.traverse(0, 4, 0, 0);
	
	codeTimer->stop();
	
	ulong numNodes = 0;
	for (int l = 0; l <= 4; ++l) numNodes += ((1<<l)*(1<<l));
	debugPrintf("sec.=%Lf   numNodes=%lu   ", (double)codeTimer->secondsPassed(), numNodes);
	debugPrintf("sec./node= %0.14Lf\n", (double)codeTimer->secondsPassed() / (double)numNodes);

	delete codeTimer;*/
/////////////////////////////////

//// TEST FLIGHT MODEL ////
//	float span = 20; //36.0833f;
//	float Croot = 8.92f;// /*5.645f;*/ 4.822f; //0.1f;
//	float Ctip = 3.92f;// /*4.0f;*/ 4.822f; //0.1f;
/*	float mach = 0.0f;
	float leSweep = 50.0f * DEGTORADf;
	float S = FlightModel::calcPlanformArea(Croot, Ctip, span);
	float AR = FlightModel::calcAspectRatio(span, S);
	float B = FlightModel::calcCompressibilityCorrection(mach);
	float sweep = atanf(FlightModel::calcTanHalfChordSweep(leSweep, Croot, Ctip, span));
	float slope = FlightModel::calcCLalpha(span, Croot, Ctip, leSweep, mach, TWO_PIf);
	debugPrintf("S = %0.5f\n", S);
	debugPrintf("AR = %0.5f\n", AR);
	debugPrintf("B = %0.5f\n", B);
	debugPrintf("c/2 sweep = %0.5f\n", sweep * RADTODEGf);
	debugPrintf("slope = %0.5f\n", slope / RADTODEGf);
*/
//	for (float m = 0.0f; m < 3.05f; m += 0.1f) {
//		slope = FlightModel::calcCLalpha(span, Croot, Ctip, leSweep, m, TWO_PIf);
//		debugPrintf("mach = %0.1f   slope = %0.5f\n", m, slope/* / RADTODEGf*/);
//	}

//// TEST AIRFOIL ////
/*	Airfoil af;
	af.mCLIntercept = 0.25f;
	af.mCLSlope = TWO_PIf;
	for (float a = 0.0f; a < 20.05f; a += 0.1f) {
		float cl = af.flowSeparationCurve(a * DEGTORADf);
	//	debugPrintf("alpha = %0.1f   cl = %0.5f\n", a, cl);
	}
*/	

/////////////////////////////////

void Application::cleanup()
{
	mScriptMgr->deinit();
	// shutdown all processes - do this early incase any processes hold Resources
	mScheduler->clear();
	mRenderer = 0;
	mScriptMgr = 0;
	mResCacheMgr = 0;
	mScheduler = 0;
	mEventMgr = 0;
}

Application::Application(
					 const string &name,
					 const Platform *pPlatform,
					 const Settings *pSettings,
					 const TimerPtr &timer,
					 const EventManagerPtr &eventMgr,
					 const SchedulerPtr &scheduler,
					 const ResCacheManagerPtr &resCacheMgr,
					 const ScriptManagerPtr &scriptMgr,
					 const RendererPtr &renderer
					) :
	appName(name),
	m_pPlatform(pPlatform),
	m_pSettings(pSettings),
	mFrameTimer(timer),
	mEventMgr(eventMgr),
	mScheduler(scheduler),
	mResCacheMgr(resCacheMgr),
	mScriptMgr(scriptMgr),
	mRenderer(renderer),
	mPausedCount(0), mExit(false)
{}

Application::~Application()
{
	cleanup();
}

// class ApplicationFactory

ApplicationUniquePtr ApplicationFactory::createIcarus()
{
	// start Timer
	TimerPtr timer(new Timer());
	timer->start();
	
	// create Event System
	unique_ptr<EventSnooper> eventSnooper(new EventSnooper());
	EventManagerPtr eventMgr(EventManager::create(eventSnooper));
	
	// create ProcessManager
	SchedulerPtr scheduler(new ProcessManager());
	
	// create Resource Cache Manager
	// TEMP these hard-coded values should come from real-time memory queries
	ResCacheManagerPtr resCacheMgr(ResCacheManager::create(2048, 512, eventMgr, scheduler));

//////////
	// create Resource Sources
	// TEMP TEST, eventually place these in a vector within Application
	ResSourcePtr srcPtr(new ZipFile(L"data/textures.zip"));
	if (srcPtr->open()) {
		resCacheMgr->registerSource(L"textures", srcPtr);
	}

	ResSourcePtr srcPtr2(new ZipFile(L"data/effects.zip"));
	if (srcPtr2->open()) {
		resCacheMgr->registerSource(L"effects", srcPtr2);
	}

	ResSourcePtr srcPtr3(new FileSystemSource(L"data/"));
	if (srcPtr3->open()) {
		resCacheMgr->registerSource(L"data", srcPtr3);
	}

	ProcessPtr tProcPtr(new TestProcess("TestProcess"));
//	CProcessPtr ttProcPtr(new TestThreadProcess("TestThreadProcess"));
//	teProcPtr->setNextProcess(ttProcPtr);
	scheduler->attach(tProcPtr);
//////////

	// create Lua Scripting System
	ScriptManagerPtr scriptMgr(new ScriptManager());
	
	// init.lua configures the startup settings
	if (!scriptMgr->init("data/script/init.lua")) {
		return ApplicationUniquePtr();
	}

	// create Renderer
	RendererPtr renderer(RendererImpl::create(m_pPlatform));
	if (!renderer->initRenderer()) {
		debugPrintf("Renderer init failed!\n");
		return ApplicationUniquePtr();
	}
// TEMP
//	activeCam  = new Camera_D3D9(Vector3f(0.0f, 0.0f, 0.0f),
//								 Vector3f(0.0f, 0.0f, 0.0f),
//								 60.0f * DEGTORADf,
//								 (float)mSettings.resXSet() / (float)mSettings.resYSet(),
//								 0.1f, 100000.0f, 1.0f);

	// create the application layer
	ApplicationUniquePtr appPtr(new Application("Icarus", m_pPlatform,
												m_pSettings, timer, eventMgr, scheduler,
												resCacheMgr, scriptMgr, renderer));
	return appPtr;
}