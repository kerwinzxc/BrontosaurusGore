#include "stdafx.h"
#include "Game.h" 

#include "Engine.h"

//STATES
#include "LoadState.h"
#include "MainMenuState.h"
#include "SplashScreen.h" //rename to state?


#include "CommonUtilities/CommandLineManager.h"
#include "LuaWrapper/SSlua/SSlua.h"
#include "ScriptLoader.h"
#include "KevinLoader/KevinLoader.h"
#include "ThreadedPostmaster/Postmaster.h"
#include "ThreadedPostmaster/PostOffice.h"
#include "PostMaster/SendNetworkMessage.h"
#include "TShared/NetworkMessage_LoadLevel.h"
#include "TClient/ClientMessageManager.h"
#include "PostMaster/Message.h"
#include "ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "TempLobbyState.h"

CGame::CGame(): mySplashScreen(nullptr)
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

	myClient.StartClient();
	//myClient.Connect("127.0.0.1", "Adam");

	//myStateStack.PushState(new CLoadState(myStateStack, 0));
	myStateStack.PushState(new CTempLobbyState(myStateStack));

	//if (CommandLineManager::GetInstance()->HasParameter("-skipMainMenu") == true)
	//{
	//	myStateStack.PushState(new CLoadState(myStateStack, 0));
	//}
	//else
	//{
	//	myStateStack.PushState(new MainMenuState(myStateStack));
	//}

	if (CommandLineManager::GetInstance()->HasParameter("-skipSplashScreen") == false)
	{
		mySplashScreen = new CSplashScreen(myStateStack);
		myStateStack.PushState(mySplashScreen);
	}


}

void CGame::Update(const CU::Time& aDeltaTime)
{
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
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