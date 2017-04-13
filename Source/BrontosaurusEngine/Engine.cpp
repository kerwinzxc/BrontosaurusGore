#include "stdafx.h"
#include "Engine.h"
#include "WindowsWindow.h"
#include "DXFramework.h"
#include "InputManager.h"
#include "ModelManager.h"
#include "SpriteManager.h"
#include "ShaderManager.h"
#include "LightManager.h"
#include "TextureManager.h"
#include <Camera.h>
#include <CommonUtilities.h>
#include <TimerManager.h>
#include <ThreadPool.h>
#include <DL_Debug.h>
#include "Skybox.h"
#include "DDSTextureLoader.h"
#include "LineDrawer.h"
#include "Renderer.h"
#include "DebugInfoDrawer.h"
#include "ThreadNamer.h"
#include "ParticleEmitterManager.h"
#include "FireEmitterManager.h"
#include "../Audio/AudioInterface.h"
#include "../FontEngine/FontEngineFacade.h"
#include "Console.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PostOffice.h"
#include "../Components/ParticleEmitterComponentManager.h"

CEngine* CEngine::myInstance = nullptr;

CEngine* CEngine::GetInstance()
{
	if(myInstance == nullptr)
	{
		myInstance = new CEngine();
	}
	return myInstance;
}

void CEngine::DestroyInstance()
{
	SAFE_DELETE(myInstance);
}

void CEngine::Init(SInitEngineParams& aInitEngineParams)
{
	CU::TimerManager timerMgr;
	CU::TimerHandle handle = timerMgr.CreateTimer();
	timerMgr.StartTimer(handle);


	myTimerManager = new CU::TimerManager();
	myTimerH = myTimerManager->CreateTimer();
	CU::ThreadPool::Create();
	myThreadPool = CU::ThreadPool::GetInstance();
	myThreadPool->Init();
	
	myWindowsWindow = new CWindowsWindow(aInitEngineParams.myWindowParams);
	myWindowSize.x = aInitEngineParams.myWindowParams.Width;
	myWindowSize.y = aInitEngineParams.myWindowParams.Height;

	myThreadRender = aInitEngineParams.myThreadRender;

	myInitCallbackFunction = aInitEngineParams.myInitCallbackFunction;
	myUpdateCallbackFunction = aInitEngineParams.myUpdateCallbackFunction;
	myRenderCallbackFunction = aInitEngineParams.myRenderCallbackFunction;

	myDXFramework = new CDXFramework();
	CU::Vector2ui actualDrawSize = myWindowsWindow->GetWindowSize();
	myWindowSize = actualDrawSize;


	myDXFramework->Initialize(actualDrawSize.x, actualDrawSize.y, aInitEngineParams.myWindowParams.Fullscreen, aInitEngineParams.myUseVsync ,myWindowsWindow->GetHWND());
	myInputManager = new CInputManager();
	myModelManager = new CModelManager();
	mySpriteManager = new CSpriteManager();
	myShaderManager = new CShaderManager();
	myLightManager = new CLightManager();
	myTextureManager = new CTextureManager();
	myFireEmitterManager = new CFireEmitterManager();

	myRenderer = new CRenderer();

	myLineDrawer = new CLineDrawer();
	CFontEngineFacade::CreateInstance();
	myFontEngine = CFontEngineFacade::GetInstance();
	myConsole = new CConsole();
	myConsole->Init();
	myDebugInfoDrawer = new CDebugInfoDrawer(aInitEngineParams.myDebugFlags);



	timerMgr.UpdateTimers();
	float time = timerMgr.GetTimer(handle).GetLifeTime().GetMilliseconds();
	ENGINE_LOG("Engine Inited in %f ms", time);


	static const std::string WeaponDirectory = "Models/Animations/M_";
	myModelManager->LoadModel(WeaponDirectory + "BFG_01.fbx");
	myModelManager->LoadModel(WeaponDirectory + "Plasma_01.fbx");
	myModelManager->LoadModel(WeaponDirectory + "Saw_01.fbx");
	myModelManager->LoadModel(WeaponDirectory + "Shotgun_01.fbx");
	myModelManager->LoadModel(WeaponDirectory + "WeaponPlayer_01_Plasma_01.fbx");
	myModelManager->LoadModel(WeaponDirectory + "WeaponPlayer_01_Shotgun_01.fbx");


	timerMgr.UpdateTimers();
	time = timerMgr.GetTimer(handle).GetDeltaTime().GetMilliseconds();
	ENGINE_LOG("Carl Inited Weapon Models in %f ms", time);
}

void CEngine::Render()
{
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
	myDXFramework->ClearDepthStencil();
	myDXFramework->ClearScreen();

	myDebugInfoDrawer->UpdateFPSCounter();


	myRenderer->Render();

	myLineDrawer->Render();
	myDXFramework->Render();
}

void CEngine::ThreadedRender()
{

		Render();
	

}

void CEngine::OnResize(const unsigned int aWidth, const unsigned int aHeight)
{
	myWindowSize.x = aWidth;
	myWindowSize.y = aHeight;

	myRenderer->Shutdown();
	if (myThreadRender == true)
	{
		while (myRendererIsRunning == true)
		{
			Sleep(1);
		}
	}
	delete myRenderer;

	myDXFramework->Resize(aWidth, aHeight);
	myDXFramework->SetViewPort(aWidth, aHeight, 0.f, 1.f, 0.f, 0.f);

	myRenderer = new CRenderer();
	if (myThreadRender == true)
	{
		auto renderThread = [this]()
		{
			ThreadedRender();
		};
		CU::Work work(renderThread, CU::ePriority::eHigh);
		std::function<bool(void)> condition = [this]()->bool{return myRenderer->GetIsRunning(); };
		work.AddLoopCondition(condition);
		work.SetName("Render thread");
		std::function<void(void)> callback = [this]() {myRendererIsRunning = false; };
		work.SetFinishedCallback(callback);
		myThreadPool->AddWork(work);
	}

}

void CEngine::Start()
{
#ifdef _WIN32
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
#elif defined __linux__
	//http ://stackoverflow.com/questions/10876342/equivalent-of-setthreadpriority-on-linux-pthreads
#endif

	if (myInitCallbackFunction != nullptr)
	{
		myInitCallbackFunction();
	}

	if (myThreadRender == true)
	{
		auto renderThread = [this]()
		{
			ThreadedRender();
		}; 
		CU::Work work(renderThread, CU::ePriority::eHigh);
		std::function<bool(void)> condition = [this]()->bool {return myRenderer->GetIsRunning(); };
		work.AddLoopCondition(condition);
		work.SetName("Render thread");
		std::function<void(void)> callback = [this]() {myRendererIsRunning = false; };
		work.SetFinishedCallback(callback);
		myThreadPool->AddWork(work);
	}

	SetForegroundWindow(myWindowsWindow->GetHWND());
	GetThreadPool()->GetInstance()->LogCreateThread();

	CU::SetThreadName("Game loop");
	while (GetWindow()->GetIsWindowOpen())
	{
		GetThreadPool()->GetInstance()->LogEnd();
		myTimerManager->UpdateTimers();
		myInputManager->Update();

		myRenderer->SwapWrite();

		myWindowsWindow->Update();
		bool consoleIsActive = false;// = myConsole->Update(myTimerManager->GetTimer(myTimerH).GetDeltaTime().GetSeconds());
		if (consoleIsActive == false)
		{
			if (myUpdateCallbackFunction(myTimerManager->GetTimer(myTimerH).GetDeltaTime()) == false)
			{
				Shutdown();
			}
		}

		myDebugInfoDrawer->Update();
		myDebugInfoDrawer->Render(myWindowSize);


		Audio::CAudioInterface* audio = Audio::CAudioInterface::GetInstance();
		if (audio != nullptr)
		{
			//audio->SetListenerPosition(myCamera->GetTransformation());
			audio->Update();
		}

	
		myRenderCallbackFunction();

		myConsole->Render();
		if (myThreadRender == false)
		{
			Render();
		}
		GetThreadPool()->GetInstance()->LogEnd();
		std::this_thread::yield();
	}

	GetThreadPool()->GetInstance()->LogDestroyThread();
}

bool CEngine::GetIsRunning()
{
	return myInstance != nullptr && myInstance->myIsRunning == true;
}

void CEngine::ClearBackbuffer()
{
	myDXFramework->ClearScreen();
}




void CEngine::Shutdown()
{
	myWindowsWindow->Close();
}

CEngine::CEngine()
	: myRenderer(nullptr)
	, myModelManager(nullptr)
	, mySpriteManager(nullptr)
	, myDXFramework(nullptr)
	, myDebugInfoDrawer(nullptr)
	, myTimerManager(nullptr)
	, myWindowsWindow(nullptr)
	, myInputManager(nullptr)
	, myShaderManager(nullptr)
	, myLightManager(nullptr)
	, myTextureManager(nullptr)
	, myLineDrawer(nullptr)
	, myThreadPool(nullptr)
	, myFireEmitterManager(nullptr)
	, myConsole(nullptr)
	, myRendererIsRunning(true)
	, myIsRunning(true)
{
}

CEngine::~CEngine()
{

	CU::ThreadPool::Destroy();
	if (myThreadRender == true)
	{
		myRenderer->Shutdown();
		while (myRendererIsRunning == true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}
	}
	SAFE_DELETE(myRenderer);
	SAFE_DELETE(myModelManager);
	SAFE_DELETE(mySpriteManager);
	SAFE_DELETE(myDXFramework);
	SAFE_DELETE(myDebugInfoDrawer);
	SAFE_DELETE(myTimerManager);
	SAFE_DELETE(myWindowsWindow);
	SAFE_DELETE(myInputManager);
	SAFE_DELETE(myShaderManager);
	SAFE_DELETE(myLightManager);
	SAFE_DELETE(myLineDrawer);
	SAFE_DELETE(myFireEmitterManager);
	SAFE_DELETE(myTextureManager);

	SAFE_DELETE(myConsole);
	myFontEngine.DestroyInstance();

	Audio::CAudioInterface::Destroy();
	myIsRunning = true;
}


CU::Time CEngine::GetTime()
{
	return myTimerManager->GetTimer(myTimerH).GetLifeTime();
}

CU::Time CEngine::GetDeltaTime()
{
	return myTimerManager->GetTimer(myTimerH).GetDeltaTime();
}
