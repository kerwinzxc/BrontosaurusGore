#include "stdafx.h"
#include "GameServer.h"
#include "../KevinLoader/KevinLoader.h"
#include "../KevinLoader/KLoaderError.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PostOffice.h"
#include "../Game/PollingStation.h"

//Managers
#include "../LoadManager/ServerLoadManager.h"
#include "../Components/ComponentManager.h"
#include "../Components/GameObjectManager.h"
#include "../Components/NetworkComponentManager.h"
#include "../Components/MovementComponentManager.h"
#include "../Components/ArenaTriggerComponentManager.h"

#include "../Components/AmmoComponentManager.h"
#include "../Components/WeaponSystemManager.h"
#include "../Components/WeaponFactory.h"
#include "../Components/DamageOnCollisionComponentManager.h"
#include "../Components/HealthComponentManager.h"
#include "../Components/SpawnerManager.h"
#include "../Game/EnemyFactory.h"
#include "../Components/CheckPointSystem.h"
#include "../Game/WaveManager.h"

//temp
#include "../Components/NetworkComponent.h"
#include "../Components/GameObject.h"
#include "../Components/ComponentMessage.h"
#include "../Components/Enemy.h"
#include "../Physics/Foundation.h"
#include "../Components/ColliderComponentManager.h"
#include "../Physics/PhysicsScene.h"
#include "../Physics/PhysicsCharacterController.h"
#include "../Components/CharacterControllerComponent.h"

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
	, mySpawnerManager(nullptr)
	,myArenaTriggerManager(nullptr)
{
	CU::ThreadPool::Create();
	myThreadPool = CU::ThreadPool::GetInstance();
	myThreadPool->Init();
	myIsRunning = false;
	myTime = 0;
	myPlayersNetworkComponents.Init(5);

	myThreadID = std::this_thread::get_id();
	myLevelIndex = -1;
}


CGameServer::~CGameServer()
{
	CNetworkComponentManager::Destroy();
	CU::ThreadPool::Destroy();
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

	ServerLoadManagerGuard loadManagerGuard(*this);


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

	myLevelIndex = aLevelIndex;
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
	myGameObjectManager->SendObjectsDoneMessage();
	CEnemyFactory::GetInstance()->LoadBluePrints(levelsArray.at(myLevelIndex).GetString());
}

void CGameServer::ReInit()
{
	DestroyManagersAndFactories();
	myIsLoaded = false;
}

void CGameServer::CreateManagersAndFactories()
{
	if (Physics::CFoundation::GetInstance() == nullptr)
	{
		Physics::CFoundation::Create();
		myPhysics = Physics::CFoundation::GetInstance()->CreatePhysics();
		myPhysicsScene = myPhysics->CreateScene();
	}

	CComponentManager::CreateInstance();
	CNetworkComponentManager::Create();
	new CPollingStation();


	myGameObjectManager = new CGameObjectManager();
	myMovementComponentManager = new CMovementComponentManager();
	myEnemyComponentManager = new CEnemyComponentManager();
	myAmmoComponentManager = new CAmmoComponentManager();
	myWeaponFactory = new CWeaponFactory();
	myWeaponSystemManager = new CWeaponSystemManager(myWeaponFactory);
	myDamageOnCollisionComponentManager = new CDamageOnCollisionComponentManager();
	mySpawnerManager = new CSpawnerManager();
	myArenaTriggerManager = new CArenaTriggerComponentManager(myThreadID);


	myColliderComponentManager = new CColliderComponentManager();
	myColliderComponentManager->SetPhysicsScene(myPhysicsScene);
	myColliderComponentManager->SetPhysics(myPhysics);
	myColliderComponentManager->InitControllerManager();
	myCheckPointSystem = new CCheckPointSystem();	
	CHealthComponentManager::Create();
	CEnemyFactory::Create(*myEnemyComponentManager,*myGameObjectManager,*myWeaponSystemManager,*myColliderComponentManager);

}

void CGameServer::DestroyManagersAndFactories()
{
	SAFE_DELETE(myEnemyComponentManager);
	CComponentManager::DestroyInstance();
	CNetworkComponentManager::Destroy();
	CHealthComponentManager::GetInstance()->Destroy();
	CEnemyFactory::Destroy();

	delete CPollingStation::GetInstance();

	myPlayersNetworkComponents.RemoveAll();

	SAFE_DELETE(myGameObjectManager);
	SAFE_DELETE(myMovementComponentManager);

	SAFE_DELETE(myAmmoComponentManager);
	SAFE_DELETE(myWeaponFactory);
	SAFE_DELETE(myWeaponSystemManager);
	SAFE_DELETE(myDamageOnCollisionComponentManager);
	SAFE_DELETE(mySpawnerManager);
	SAFE_DELETE(myCheckPointSystem);
	SAFE_DELETE(myArenaTriggerManager);
}

bool CGameServer::Update(CU::Time aDeltaTime)
{


	//DL_PRINT("In Update");
	myTime += aDeltaTime.GetMilliseconds();

	const float updateFrequecy = 0.016f * 1000;
	if(myTime > updateFrequecy)
	{
		if (myIsLoaded == true && myEnemyComponentManager->GetIsInited() == true)
		{
			myEnemyComponentManager->Update(aDeltaTime.GetSeconds() + (updateFrequecy / 1000.0f));
		}
		//mySpawnerManager->Update(aDeltaTime.GetSeconds() + (updateFrequecy / 1000.0f));

		if (myIsLoaded == true)
		{
			myArenaTriggerManager->Update();
		}
		myTime = 0;
	}

	CPollingStation* instance = CPollingStation::GetInstance();
	
	if (myPhysicsScene->Simulate(aDeltaTime + (updateFrequecy / 1000.0f)) == true)
	{
		myColliderComponentManager->Update();
	}
	myDamageOnCollisionComponentManager->Update(aDeltaTime);
	if (myIsLoaded == true && myEnemyComponentManager->GetIsInited() == false)
	{
		myEnemyComponentManager->Init(myWeaponSystemManager);
	}
	for(unsigned int i = 0; i < myPlayersNetworkComponents.Size(); i++)
	{
		myPlayersNetworkComponents[i]->Update();
	}
	myWeaponSystemManager->Update(aDeltaTime);
	return true;
}

bool CGameServer::IsLoaded() const
{
	return myIsLoaded;
}

std::thread::id & CGameServer::GetThreadID()
{
	return myThreadID;
}

CServerPlayerNetworkComponent* CGameServer::AddPlayer(const unsigned short aClientID)
{
	CGameObject*const gameObject = myGameObjectManager->CreateGameObject();

	CServerPlayerNetworkComponent*const serverPlayerNetworkComponent = new CServerPlayerNetworkComponent;
	serverPlayerNetworkComponent->SetClientID(aClientID);
	myPlayersNetworkComponents.Add(serverPlayerNetworkComponent);
	CComponentManager::GetInstance().RegisterComponent(serverPlayerNetworkComponent);
	gameObject->AddComponent(serverPlayerNetworkComponent);
	Physics::SCharacterControllerDesc controllerDesc;
	controllerDesc.minMoveDistance = 0.00001f;
	controllerDesc.halfHeight = 1.0f;
	CCharacterControllerComponent* controller = myColliderComponentManager->CreateCharacterControllerComponent(controllerDesc, gameObject->GetId());
	gameObject->AddComponent(controller);
	CEnemy::SetPlayerObject(gameObject);
	gameObject->NotifyComponents(eComponentMessageType::eObjectDone, SComponentMessageData());

	CPollingStation::GetInstance()->AddPlayerObject(gameObject);

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

CSpawnerManager * CGameServer::GetSpawnerManager()
{
	return mySpawnerManager;
}
