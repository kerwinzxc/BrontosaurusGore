#include "stdafx.h"
#include "ServerLoadManager.h"
#include "LoadObject.h"



CServerLoadManager* CServerLoadManager::ourInstance = nullptr;

void CServerLoadManager::CreateInstance(CGameServer& aGameServer)
{
	assert(ourInstance == nullptr && "Load manager already created");
	ourInstance = new CServerLoadManager(aGameServer);
}

CServerLoadManager* CServerLoadManager::GetInstance()
{
	return ourInstance;
}

void CServerLoadManager::DestroyInstance()
{
	assert(ourInstance != nullptr && "Load manager not created (is NULL)");
	SAFE_DELETE(ourInstance);
}

//void LoadManager::SetCurrentScene(CScene* aScene)
//{
//	myCurrentScene = aScene;
//}

//void LoadManager::SetCurrentPlayState(CPlayState* aPlaystate)
//{
//	myCurrentPlaystate = aPlaystate;
//}

CGameServer& CServerLoadManager::GetCurrentGameServer() const
{
	return myCurrentGameServer;
}

void CServerLoadManager::RegisterFunctions()
{
	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();

	loader.RegisterObjectLoadFunction(LoadObject);
	loader.RegisterObjectLinkFunction(LinkObject);

	//loader.RegisterComponentLoadFunction("MeshFilter", LoadMeshFilter);
	//loader.RegisterComponentLoadFunction("Camera", LoadCamera);
	//loader.RegisterComponentLoadFunction("CircleCollider", LoadCircleCollider);
	//loader.RegisterComponentLoadFunction("SoundComponent", LoadSoundComponent);
	//loader.RegisterComponentLoadFunction("LuaScriptComponent", LoadScriptComponent);
	//loader.RegisterComponentLoadFunction("FireComponent", LoadFireComponent);
	//loader.RegisterComponentLoadFunction("PointLightComponent", LoadPointLightComponent);
}

CServerLoadManager::CServerLoadManager(CGameServer& aGameServer)
	: myCurrentGameServer(aGameServer)
{
	//myCurrentScene = nullptr;
	//myCurrentPlaystate = nullptr;

	RegisterFunctions();
}


CServerLoadManager::~CServerLoadManager()
{
	//myCurrentScene = nullptr;
	//myCurrentPlaystate = nullptr;
}