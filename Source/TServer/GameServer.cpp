#include "stdafx.h"
#include "GameServer.h"
#include "../Components/ComponentManager.h"
#include "../Components/GameObjectManager.h"
#include "../Components/AmmoComponentManager.h"
#include "../Components/WeaponFactory.h"
#include "../Components/WeaponSystemManager.h"
#include "../Components/NetworkComponentManager.h"
#include "../KevinLoader/KevinLoader.h"
#include "../KevinLoader/KLoaderError.h"
#include "../LoadManager/ServerLoadManager.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PostOffice.h"





CGameServer::CGameServer(): myAmmoComponentManager(nullptr), myGameObjectManager(nullptr), myWeaponFactory(nullptr), myMainTimer(0), myInGame(false), myIsLoaded(false), myWeaponSystemManager(nullptr)
{
	myIsRunning = false;
}


CGameServer::~CGameServer()
{
	delete myNetworkComponentManager;
	myNetworkComponentManager = nullptr;
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

CNetworkComponentManager & CGameServer::GetNetworkComponentManager()
{
	return *myNetworkComponentManager;
}

void CGameServer::Load(const int aLevelIndex)
{
	ServerLoadManagerGuard loadManagerGuard(*this);
	CreateManagersAndFactories();


	CU::TimerManager timerMgr;
	CU::TimerHandle handle = timerMgr.CreateTimer();
	timerMgr.StartTimer(handle);

	srand(static_cast<unsigned int>(time(nullptr)));



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
	int myLevelIndex = aLevelIndex;
	std::string levelPath = "Json/Levels/";
	levelPath += levelsArray[myLevelIndex].GetString();
	levelPath += "/LevelData.json";

	const KLoader::eError loadError = loader.LoadFile(levelPath);
	if (loadError != KLoader::eError::NO_LOADER_ERROR)
	{
		DL_MESSAGE_BOX("Loading Failed");
	}
	myIsLoaded = true;
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
}

void CGameServer::CreateManagersAndFactories()
{
	CComponentManager::CreateInstance();

	myGameObjectManager = new CGameObjectManager();

	myAmmoComponentManager = new CAmmoComponentManager();
	myWeaponFactory = new CWeaponFactory();
	myWeaponSystemManager = new CWeaponSystemManager(myWeaponFactory);

	myNetworkComponentManager = new CNetworkComponentManager;
}

bool CGameServer::Update(CU::Time aDeltaTime)
{
	//DL_PRINT("In Update");
	return true;
}

bool CGameServer::IsLoaded() const
{
	return myIsLoaded;
}
