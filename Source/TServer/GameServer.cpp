#include "stdafx.h"
#include "GameServer.h"
#include "../KevinLoader/KevinLoader.h"
#include "../KevinLoader/KLoaderError.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PostOffice.h"

//Managers
#include "../LoadManager/ServerLoadManager.h"
#include "../Components/ComponentManager.h"
#include "../Components/GameObjectManager.h"
#include "../Components/NetworkComponentManager.h"
#include "../Components/MovementComponentManager.h"

#include "../Components/AmmoComponentManager.h"
#include "../Components/WeaponSystemManager.h"
#include "../Components/WeaponFactory.h"

//temp
#include "../Components/NetworkComponent.h"
#include "../Components/GameObject.h"
#include "../Components/ComponentMessage.h"
#include "../Components/Enemy.h"
#include "../Physics/Foundation.h"
#include "../Components/ColliderComponentManager.h"
#include "../Physics/PhysicsScene.h"


CGameServer::CGameServer():
	myAmmoComponentManager(nullptr)
	, myGameObjectManager(nullptr)
	, myWeaponFactory(nullptr)
	, myMainTimer(0)
	, myInGame(false)
	, myIsLoaded(false)
	, myEnemyComponentManager(nullptr)
	, myPhysics(nullptr)
	, myPhysicsScene(nullptr)
	, myWeaponSystemManager(nullptr)
	, myMovementComponentManager(nullptr)
{
	myIsRunning = false;
	myTime = 0;
}


CGameServer::~CGameServer()
{
	CNetworkComponentManager::Destroy();
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

void CGameServer::Load(const int aLevelIndex)
{
	if (Physics::CFoundation::GetInstance() == nullptr) Physics::CFoundation::Create();

	myPhysics = Physics::CFoundation::GetInstance()->CreatePhysics();
	myPhysicsScene = myPhysics->CreateScene();

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

void CGameServer::ReInit()
{
	DestroyManagersAndFactories();
	myIsLoaded = false;
}

void CGameServer::CreateManagersAndFactories()
{
	CComponentManager::CreateInstance();
	CNetworkComponentManager::Create();

	myGameObjectManager = new CGameObjectManager();
	myMovementComponentManager = new CMovementComponentManager();
	myEnemyComponentManager = new CEnemyComponentManager();

	myAmmoComponentManager = new CAmmoComponentManager();
	myWeaponFactory = new CWeaponFactory();
	myWeaponSystemManager = new CWeaponSystemManager(myWeaponFactory);

	myColliderComponentManager = new CColliderComponentManager();
	myColliderComponentManager->SetPhysicsScene(myPhysicsScene);
	myColliderComponentManager->SetPhysics(myPhysics);
	myColliderComponentManager->InitControllerManager();
}

void CGameServer::DestroyManagersAndFactories()
{
	CComponentManager::DestroyInstance();
	CNetworkComponentManager::Destroy();

	SAFE_DELETE(myGameObjectManager);
	SAFE_DELETE(myMovementComponentManager);

	SAFE_DELETE(myAmmoComponentManager);
	SAFE_DELETE(myWeaponFactory);
	SAFE_DELETE(myWeaponSystemManager);
	SAFE_DELETE(myEnemyComponentManager);
}

bool CGameServer::Update(CU::Time aDeltaTime)
{
	//DL_PRINT("In Update");
	myTime += aDeltaTime.GetMilliseconds();

	const float updateFrequecy = 0.016f * 1000;
	if(myTime > updateFrequecy)
	{
		myEnemyComponentManager->Update(aDeltaTime);
		myTime = 0;
	}
	if (myPhysicsScene->Simulate(aDeltaTime) == true)
	{
		myColliderComponentManager->Update();
	}

	return true;
}

bool CGameServer::IsLoaded() const
{
	return myIsLoaded;
}

CServerPlayerNetworkComponent* CGameServer::AddPlayer() const
{
	CGameObject*const gameObject = myGameObjectManager->CreateGameObject();

	CServerPlayerNetworkComponent*const serverPlayerNetworkComponent = new CServerPlayerNetworkComponent;
	CComponentManager::GetInstance().RegisterComponent(serverPlayerNetworkComponent);
	gameObject->AddComponent(serverPlayerNetworkComponent);

	Component::CEnemy::SetPlayer(gameObject);

	return serverPlayerNetworkComponent;
}

CEnemyComponentManager* CGameServer::GetEnemyComponentManager()
{
	return myEnemyComponentManager;
}

CWeaponSystemManager* CGameServer::GetCWeaponSystemManager()
{
	return myWeaponSystemManager;
}
