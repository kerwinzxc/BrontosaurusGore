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
	myThreadPool = new CU::ThreadPool();
	
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
	myParticleEmitterManager = new CParticleEmitterManager();
	myFireEmitterManager = new CFireEmitterManager();

	myRenderer = new CRenderer();

	myLineDrawer = new CLineDrawer();
	CFontEngineFacade::CreateInstance();
	myFontEngine = CFontEngineFacade::GetInstance();
	myConsole = new CConsole();
	myConsole->Init();
	myDebugInfoDrawer = new CDebugInfoDrawer(aInitEngineParams.myDebugFlags);

	bool result;
	Audio::CAudioInterface::CreateInstance();
	result = Audio::CAudioInterface::GetInstance()->Init("Audio/Init.bnk");

	timerMgr.UpdateTimers();
	float time = timerMgr.GetTimer(handle).GetLifeTime().GetMilliseconds();
	ENGINE_LOG("Engine Inited in %f ms", time);
}

void CEngine::Render()
{
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
	myDXFramework->ClearDepthStencil();
	myDXFramework->ClearScreen();

	//myDebugInfoDrawer->UpdateFPSCounter();


	myRenderer->Render();

	myLineDrawer->Render();
	myDXFramework->Render();
}

void CEngine::ThreadedRender()
{
	myRendererIsRunning = true;
	CU::SetThreadName("Render thread");
	while (myRenderer->GetIsRunning() == true)
	{
		Render();
	}


	myRendererIsRunning = false;
	CU::SetThreadName("ThreadPool Worker");

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
		myThreadPool->AddWork(CU::Work(renderThread, CU::ePriority::eHigh));
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
		myThreadPool->AddWork(CU::Work(renderThread, CU::ePriority::eHigh));
	}

	SetForegroundWindow(myWindowsWindow->GetHWND());

	CParticleEmitterComponentManager::Create();
	while (GetIsRunning())
	{
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

		//myDebugInfoDrawer->Update();
		//myDebugInfoDrawer->Render(myWindowSize);


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
	}
	CParticleEmitterComponentManager::Destroy();
}

bool CEngine::GetIsRunning()
{
	return myWindowsWindow->GetIsWindowOpen();
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
	, myParticleEmitterManager(nullptr)
	, myFireEmitterManager(nullptr)
	, myConsole(nullptr)
	, myRendererIsRunning(false)
{
}

CEngine::~CEngine()
{
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
	SAFE_DELETE(myThreadPool); //TODO: THREAD POOL HAS THREADS IT CANNOT JOIN, don't know if this is true anymore
	SAFE_DELETE(myParticleEmitterManager);
	SAFE_DELETE(myFireEmitterManager);
	SAFE_DELETE(myTextureManager);

	SAFE_DELETE(myConsole);
	myFontEngine.DestroyInstance();

	Audio::CAudioInterface::Destroy();
}


CU::Time CEngine::GetTime()
{
	return myTimerManager->GetTimer(myTimerH).GetLifeTime();
}

CU::Time CEngine::GetDeltaTime()
{
	return myTimerManager->GetTimer(myTimerH).GetDeltaTime();
}
