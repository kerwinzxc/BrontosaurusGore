#include "stdafx.h"
#include "Game.h" 

#include "Engine.h"

//STATES
#include "PlayState.h"
#include "MainMenuState.h"
#include "SplashScreen.h" //rename to state?
//


//Should these stuffs be here?
#include "CommonUtilities/CommandLineManager.h"
#include "LoadManager/LuaFunctions.h"
#include "LuaWrapper/SSlua/SSlua.h"
#include "ScriptLoader.h"
#include "KevinLoader/KLoaderError.h"  // PLZ not the kevin prefix
#include "KevinLoader/KevinLoader.h"   // PLZ not the kevin prefix

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

	 SSlua::LuaWrapper& luaWrapper = SSlua::LuaWrapper::GetInstance();
	 luaWrapper.RegisterFunctions(&ScriptLoader::RegisterLuaFunctions);
	 

	if (CommandLineManager::GetInstance()->HasParameter("-skipMainMenu") == true)
	{
		myStateStack.PushState(new CPlayState(myStateStack, 0));
	}
	else
	{
		myStateStack.PushState(new MainMenuState(myStateStack));
	}
	
	if (CommandLineManager::GetInstance()->HasParameter("-skipSplashScreen") == false)
	{
		mySplashScreen = new CSplashScreen(myStateStack);
		myStateStack.PushState(mySplashScreen);
	}
}

void CGame::Update(const CU::Time& aDeltaTime)
{
	bool isRunning = myStateStack.Update(aDeltaTime);
	if (isRunning == false)
	{
		CEngine::GetInstance()->Shutdown();
	}		
}

void CGame::Render()
{
	myStateStack.Render();
}