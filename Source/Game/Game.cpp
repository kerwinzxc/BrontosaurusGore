#include "stdafx.h"
#include "Game.h" 

//STATES
#include "SplashScreen.h"
#include "TempLobbyState.h"

#include "CommonUtilities/CommandLineManager.h"
#include "LuaWrapper/SSlua/SSlua.h"
#include "ScriptLoader.h"
#include "KevinLoader/KevinLoader.h"
#include "BackgroundLoadingManager.h"
#include "ThreadedPostmaster/Postmaster.h"


#include "../CommonUtilities/WindowsHelper.h"
#include "MenuState.h"

CGame::CGame()
{
}

CGame::~CGame()
{
	KLoader::CKevinLoader::DestroyInstance();
	CBackgroundLoadingManager::DestroyInstance();
	SSlua::LuaWrapper::DestroyIfCreated();

	//WindowsHelper::CloseProgram("TServer_Applictaion_x64_Release.exe");
}


void CGame::Init()
{
	//WindowsHelper::StartProgram("TServer_Applictaion_x64_Release.exe");
	CBackgroundLoadingManager::CreateInstance();
	KLoader::CKevinLoader::CreateInstance();
	SSlua::LuaWrapper::GetInstance().RegisterFunctions(&ScriptLoader::RegisterLuaFunctions);
	myGameEventMessenger.Init({ 0.5f, 0.1f });
	myClient.StartClient();

	myStateStack.PushState(new CMenuState(myStateStack, "Json/Menu/MainMenu.json"));

	if (CommandLineManager::GetInstance()->HasParameter("-skipSplashScreen") == false)
	{
		mySplashScreenState = new CSplashScreenState(myStateStack);

		mySplashScreenState->AddPicture("Sprites/SplashScreen/Splash1.dds");
		mySplashScreenState->AddPicture("Sprites/SplashScreen/Splash2.dds");
		mySplashScreenState->AddPicture("Sprites/SplashScreen/Splash3.dds");
		myStateStack.PushState(mySplashScreenState);
	}

	//myStateStack.PushState(new CTempLobbyState(myStateStack));
}

bool CGame::Update(const CU::Time& aDeltaTime)
{
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
	myGameEventMessenger.Update(aDeltaTime.GetSeconds());
	if (myStateStack.Update(aDeltaTime) == false)
	{
		return false;
	}

	return true;
}

void CGame::Render()
{
	myStateStack.Render();
	myGameEventMessenger.Render();
}