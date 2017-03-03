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

	myClient.StartClient();
	myClient.Connect("127.0.0.1", "Adam");

	if (myClient.GetID() == ePreDefId::ID_FREE)
	{
		CClientMessageManager* messageMangaerInstance = CClientMessageManager::GetInstance();

		if (messageMangaerInstance == nullptr)
		{
			DL_ASSERT("clientMessageManager not created");
		}

		CNetworkMessage_LoadLevel* loadLevelMessage = messageMangaerInstance->CreateMessage<CNetworkMessage_LoadLevel>("__All_But_Me");

		loadLevelMessage->myLevelIndex = 0;

		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(loadLevelMessage));
	}

	myStateStack.PushState(new CLoadState(myStateStack, 0));
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