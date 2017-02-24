#include "stdafx.h"
#include "GameServer.h"
#include "../Components/ComponentManager.h"
#include "../Components/GameObjectManager.h"
#include "../Components/AmmoComponentManager.h"
#include "../Components/WeaponFactory.h"
#include "../Components/WeaponSystemManager.h"
#include "../KevinLoader/KevinLoader.h"
#include "../KevinLoader/KLoaderError.h"
#include "../LoadManager/ServerLoadManager.h"





CGameServer::CGameServer():myAmmoComponentManager(nullptr),myGameObjectManager(nullptr),myWeaponFactory(nullptr),myWeaponSystemManager(nullptr)
{
	myIsRunning = false;
}


CGameServer::~CGameServer()
{
}

void CGameServer::Init()
{
	
}

void CGameServer::Start()
{
	
}

CGameObjectManager & CGameServer::GetGameObjectManager()
{
	return *myGameObjectManager;
}

void CGameServer::Load()
{
	CU::TimerManager timerMgr;
	CU::TimerHandle handle = timerMgr.CreateTimer();
	timerMgr.StartTimer(handle);

	srand(static_cast<unsigned int>(time(nullptr)));

	ServerLoadManagerGuard loadManagerGuard(*this);
	CreateManagersAndFactories();

	myWeaponFactory->LoadWeapons();

	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();
	CU::CJsonValue levelsFile;

	std::string errorString = levelsFile.Parse("Json/LevelList.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	CU::CJsonValue levelsArray = levelsFile.at("levels");

#ifdef _DEBUGq
	myLevelIndex = levelsArray.Size() - 1;
#else
	const int levelIndex = 0;
#endif
	int myLevelIndex = 1;
	std::string levelPath = "Json/Levels/";
	levelPath += levelsArray[myLevelIndex].GetString();
	levelPath += "/LevelData.json";

	const KLoader::eError loadError = loader.LoadFile(levelPath);
	if (loadError != KLoader::eError::NO_LOADER_ERROR)
	{
		DL_MESSAGE_BOX("Loading Failed");
	}
	myIsLoaded = true;

}

void CGameServer::CreateManagersAndFactories()
{
	CComponentManager::CreateInstance();

	myGameObjectManager = new CGameObjectManager();

	myAmmoComponentManager = new CAmmoComponentManager();
	myWeaponFactory = new CWeaponFactory();
	myWeaponSystemManager = new CWeaponSystemManager(myWeaponFactory);
}

bool CGameServer::Update()
{
	return true;
}
