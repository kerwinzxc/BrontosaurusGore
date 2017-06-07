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
#include "EnemyClientRepresentationManager.h"

CGame::CGame()
{
}

CGame::~CGame()
{
	KLoader::CKevinLoader::DestroyInstance();
	CBackgroundLoadingManager::DestroyInstance();
	SSlua::LuaWrapper::DestroyIfCreated();
}


void CGame::Init()
{
	CBackgroundLoadingManager::CreateInstance();
	KLoader::CKevinLoader::CreateInstance();
	SSlua::LuaWrapper::GetInstance().RegisterFunctions(&ScriptLoader::RegisterLuaFunctions);
	myGameEventMessenger.Init({ 0.5f, 0.1f });
	myClient.StartClient();

	myStateStack.SetCleanupCallback([this]() { CleanUpNow(); });
	myStateStack.PushState(new CMenuState(myStateStack, "Json/Menu/MainMenu.json"));

	if (CommandLineManager::GetInstance()->HasParameter("-skipSplashScreen") == false)
	{
		mySplashScreenState = new CSplashScreenState(myStateStack);

		mySplashScreenState->AddPicture("Sprites/Splash/tga.dds");
		mySplashScreenState->AddPicture("Sprites/Splash/timedust.dds");
		myStateStack.PushState(mySplashScreenState);
	}
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

void CGame::CleanUpNow()
{
	myClient.CleanUpNow();
	CEnemyClientRepresentationManager::GetInstance().CleanUpRepresentations();
}
