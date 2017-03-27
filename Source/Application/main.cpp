#include "stdafx.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../Physics/SharedPhysicsPointer.h"
#include "../Physics/PhysXManager.h"
#include "ParticleEmitterManager.h"
#include "ParticleEmitterComponentManager.h"


void Init(int argc, char* argv[]);
unsigned int DebugDrawerFlags();


#ifndef _RETAIL_BUILD
//Comment out linebelow to deactivate console
#define USE_CONSOLE
#endif // !_RETAIL_BUILD

#ifdef USE_CONSOLE
	#pragma comment(linker, "/SUBSYSTEM:console")
#else
	#include <windows.h>
	#pragma comment(linker, "/SUBSYSTEM:windows")
#endif

#ifdef USE_CONSOLE
int main(int argc, char* argv[])
{
	Init(argc, argv);
	return 0;
}
#else
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
	Init(__argc, __argv);
	return 0;
}
#endif


void InitAudio();

void Init(int argc, char* argv[])
{
	

	Postmaster::Threaded::CPostmaster::Create();
	CommandLineManager::CreateInstance(argc, argv);

	

	DL_Debug::Debug::CreateInstance();
	InitAudio();
	PostMaster::CreateInstance();

	{
		CGame game;
		
		SInitEngineParams engineParams;
		engineParams.myWindowParams.Width = 1920;
		engineParams.myWindowParams.Height = 1080;
		engineParams.myWindowParams.Name = L"HighDoom";
		engineParams.myWindowParams.Title = L"HighDoom";
		engineParams.myWindowParams.Fullscreen = false;
		engineParams.myThreadRender = true;
		engineParams.myUseVsync = false;

		int Iwidth;
		int Iheight;
		bool Bfullscreen = true;

		if (CommandLineManager::GetInstance()->HasParameter("-width") == true)
		{
			std::string width;
			width = CommandLineManager::GetInstance()->GetArgument("-width");
			Iwidth = std::atoi(width.c_str());
			engineParams.myWindowParams.Width = static_cast<unsigned short>(Iwidth);
		}
		if (CommandLineManager::GetInstance()->HasParameter("-height") == true)
		{
			std::string height;
			height = CommandLineManager::GetInstance()->GetArgument("-height");
			Iheight = std::atoi(height.c_str());
			engineParams.myWindowParams.Height = static_cast<unsigned short>(Iheight);
		}
		if (CommandLineManager::GetInstance()->HasParameter("-fullscreen") == true)
		{
			std::string fullscreen;
			fullscreen = CommandLineManager::GetInstance()->GetArgument("-fullscreen");
			if (fullscreen == "False")
			{
				Bfullscreen = false;
				//std::cout << "false" << std::endl;
			}
			else if (fullscreen == "True")
			{
				Bfullscreen = true;
				//std::cout << "True" << std::endl;
			}
			engineParams.myWindowParams.Fullscreen = Bfullscreen;
		}
		if (CommandLineManager::GetInstance()->HasParameter("-noThreadRender") == true)
		{
			std::string fullscreen;
			fullscreen = CommandLineManager::GetInstance()->GetArgument("-noThreadRender");

			engineParams.myThreadRender = false;
		}



		engineParams.myInitCallbackFunction = std::bind(&CGame::Init, &game);
		engineParams.myUpdateCallbackFunction = std::bind(&CGame::Update, &game, std::placeholders::_1);
		engineParams.myRenderCallbackFunction = std::bind(&CGame::Render, &game);
		engineParams.myDebugFlags = DebugDrawerFlags();

		
		CEngine::GetInstance()->Init(engineParams);
		//Physics::CPhysXManager::Create();
		CEngine::GetInstance()->Start();
	}

	Audio::CAudioInterface::Destroy();
	CEngine::DestroyInstance();
	PostMaster::DestroyInstance();
	DL_Debug::Debug::DestroyInstance();
	CommandLineManager::DestroyInstance();

	Postmaster::Threaded::CPostmaster::Destroy();
	//DumpMemoryLeeks();
}

unsigned int DebugDrawerFlags()
{
#ifdef _DEBUG
	return eDebugFlags_FPS | eDebugFlags_LogicFPS | eDebugFlags_DrawCalls | eDebugFlags_MemoryUsage | eDebugFlags_RoundTripTime | eDebugFlags_DataAmmountSent;
#else //NDEBUG || _RETAIL_BUILD
	return 0;
#endif // _DEBUG
}

void WWiseErrorCallback(const char* anError)
{
	std::string errorText = "WWise error: ";
	errorText += anError;
	DL_ASSERT(errorText.c_str());
}

void InitAudio()
{
	Audio::CAudioInterface::CreateInstance();

	Audio::CAudioInterface::GetInstance()->SetErrorCallBack(WWiseErrorCallback);
	bool result;

	std::string bankPath = "Audio/Init.bnk";

	result = Audio::CAudioInterface::GetInstance()->Init(bankPath.c_str());

	bankPath = "Audio/Feedback.bnk";
	result = Audio::CAudioInterface::GetInstance()->LoadBank(bankPath.c_str());

}
