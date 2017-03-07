#include "stdafx.h"
#include "Game.h" 

//STATES
//#include "SplashScreen.h"
#include "TempLobbyState.h"

#include "CommonUtilities/CommandLineManager.h"
#include "LuaWrapper/SSlua/SSlua.h"
#include "ScriptLoader.h"
#include "KevinLoader/KevinLoader.h"
#include "BackgroundLoadingManager.h"
#include "ThreadedPostmaster/Postmaster.h"

//TEMP INCLUDES??
//#include "ThreadedPostmaster/PostOffice.h"
//#include "PostMaster/SendNetworkMessage.h"
//#include "TShared/NetworkMessage_LoadLevel.h"
//#include "TClient/ClientMessageManager.h"
//#include "PostMaster/Message.h"
//#include "ThreadedPostmaster/SendNetowrkMessageMessage.h"

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

	myClient.StartClient();
	//myClient.Connect("127.0.0.1", "Adam");

	myStateStack.PushState(new CTempLobbyState(myStateStack));

	//if (CommandLineManager::GetInstance()->HasParameter("-skipSplashScreen") == false)
	//{
	//	myStateStack.PushState(new CSplashScreenState(myStateStack));
	//}
}

bool CGame::Update(const CU::Time& aDeltaTime)
{
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();

	if (myStateStack.Update(aDeltaTime) == false)
	{
		return false;
	}

	return true;
}

void CGame::Render()
{
	myStateStack.Render();
}