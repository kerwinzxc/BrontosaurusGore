#include "stdafx.h"
#include "PlayState.h"

#include <Engine.h>
#include <CommonUtilities.h>
#include <StopWatch.h>
#include <EInputReturn.h>
#include <Lights.h>
#include <Scene.h>

#include "Skybox.h"
#include "../Audio/AudioInterface.h"

#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/Event.h"


//Managers

#include "Components/GameObject.h"
#include "Components/GameObjectManager.h"
#include "Components/ComponentManager.h"
#include "Components/AmmoComponentManager.h"
#include "Components//WeaponSystemManager.h"
#include "Components/WeaponFactory.h"
#include "Components/ProjectileComponentManager.h"
#include "Components/ProjectileFactory.h"
#include "Components/NetworkComponentManager.h"
#include "Components/MovementComponentManager.h"
//#include "../GUI/GUIManager.h"

#include "LoadManager/LoadManager.h"
#include "KevinLoader/KevinLoader.h"

#include "CameraComponent.h"
#include "InputComponent.h"
#include <thread>
#include "MovementComponent.h"
#include "ModelComponentManager.h"
#include "EnemyComponentManager.h"
#include "Enemy.h"

#include "WeaponSystemComponent.h"
#include "AmmoComponent.h"
#include "ComponentMessage.h"
#include "TClient/Client.h"
#include "TShared/NetworkMessage_ClientReady.h"
#include "TClient/ClientMessageManager.h"
#include "PostMaster/SendNetworkMessage.h"
#include "ThreadedPostmaster/Postmaster.h"
#include "ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "PauseMenu.h"
#include "StateStack/StateStack.h"
#include "CommonUtilities/InputMessage.h"
#include <CommonUtilities/EKeyboardKeys.h>


#include "../Components/PlayerNetworkComponent.h"

//Hard code necessary includes
#include "AmmoReplenishData.h"
#include "ThreadedPostmaster/OtherPlayerSpawned.h"
//

CPlayState::CPlayState(StateStack& aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::ePlayState, 1)
	, myLevelIndex(aLevelIndex)
	, myGameObjectManager(nullptr)
	, myScene(nullptr)
	, myModelComponentManager(nullptr)
	, myMovementComponent(nullptr)
	, myCameraComponent(nullptr)
	, myAmmoComponentManager(nullptr)
	, myWeaponFactory(nullptr)
	, myWeaponSystemManager(nullptr)
	, myProjectileComponentManager(nullptr)
	, myProjectileFactory(nullptr)
	, myInputComponentManager(nullptr)
	, myMovementComponentManager(nullptr)
	, myIsLoaded(false)
{
}

CPlayState::~CPlayState()
{
	SAFE_DELETE(myGameObjectManager);
	SAFE_DELETE(myScene);

	SAFE_DELETE(myModelComponentManager);

	SAFE_DELETE(myAmmoComponentManager);
	SAFE_DELETE(myWeaponFactory);
	SAFE_DELETE(myProjectileComponentManager);
	SAFE_DELETE(myProjectileFactory);
	SAFE_DELETE(myMovementComponentManager);
	SAFE_DELETE(myEnemyComponentManager);

	CNetworkComponentManager::Destroy();

	CComponentManager::DestroyInstance();
}

void CPlayState::Load()
{
	CU::CStopWatch loadPlaystateTimer;
	loadPlaystateTimer.Start();

	srand(static_cast<unsigned int>(time(nullptr)));

	CU::CJsonValue levelsFile;
	std::string errorString = levelsFile.Parse("Json/LevelList.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	CU::CJsonValue levelsArray = levelsFile.at("levels");

	if (!levelsArray.HasIndex(myLevelIndex))
	{
		DL_MESSAGE_BOX("Tried to load level with index out of range (%d), level count is %d", myLevelIndex, levelsArray.Size());
		return;
	}

	std::string levelPath = "Json/Levels/";
	levelPath += levelsArray[myLevelIndex].GetString();
	levelPath += "/LevelData.json";

	CreateManagersAndFactories();
	LoadManagerGuard loadManagerGuard(*this, *myScene);

	Lights::SDirectionalLight dirLight;
	dirLight.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	dirLight.direction = { -1.0f, -1.0f, 1.0f, 1.0f };
	dirLight.shadowIndex = 0;
	myScene->AddDirectionalLight(dirLight);


	myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	CU::Camera& playerCamera = myScene->GetCamera(CScene::eCameraType::ePlayerOneCamera);
	playerCamera.Init(110, WINDOW_SIZE_F.x, WINDOW_SIZE_F.y, 0.1f, 1000.f);
	
	myWeaponFactory->LoadWeapons();




	//real loading:		as opposed to fake loading
	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();

	const KLoader::eError loadError = loader.LoadFile(levelPath);
	if (loadError != KLoader::eError::NO_LOADER_ERROR)
	{
		DL_MESSAGE_BOX("Loading Failed");
	}

	TempHardCodePlayerRemoveTHisLaterWhenItIsntNecessaryToHaveAnymore(playerCamera); // Hard codes Player!;
	
	myGameObjectManager->SendObjectsDoneMessage();

	myScene->SetSkybox("default_cubemap.dds");
	myScene->SetCubemap("purpleCubemap.dds");
	
	myIsLoaded = true;
	
	// Get time to load the level:
	loadPlaystateTimer.Update();
	float time = loadPlaystateTimer.GetDeltaTime().GetMilliseconds();
	GAMEPLAY_LOG("Game Inited in %f ms", time);
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
}

void CPlayState::Init()
{
}

eStateStatus CPlayState::Update(const CU::Time& aDeltaTime)
{
	myMovementComponentManager->Update(aDeltaTime);
	myEnemyComponentManager->Update(aDeltaTime);
	myWeaponSystemManager->Update(aDeltaTime);
	myProjectileComponentManager->Update(aDeltaTime);
	myProjectileFactory->Update(aDeltaTime.GetSeconds());
	myAmmoComponentManager->Update(aDeltaTime);

	myScene->Update(aDeltaTime);

	return myStatus;
}

void CPlayState::Render()
{
	myScene->Render();
}

void CPlayState::OnEnter(const bool /*aLetThroughRender*/)
{
}

void CPlayState::OnExit(const bool /*aLetThroughRender*/)
{
}

void CPlayState::Pause()
{
	myStateStack.PushState(new CPauseMenuState(myStateStack));
}

eMessageReturn CPlayState::Recieve(const Message& aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

CU::eInputReturn CPlayState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eKeyboardPressed && aInputMessage.myKey == CU::eKeys::ESCAPE)
	{
		Pause();
	}
	else
	{
		CU::CInputMessenger::RecieveInput(aInputMessage);
	}
	
	return CU::eInputReturn::eKeepSecret;
}

CGameObjectManager* CPlayState::GetGameObjectManager()
{
	return myGameObjectManager;
}

void CPlayState::CreateManagersAndFactories()
{
	CComponentManager::CreateInstance();

	CNetworkComponentManager::Create();

	myScene = new CScene();

	myGameObjectManager = new CGameObjectManager();
	myModelComponentManager = new CModelComponentManager(*myScene);
	myEnemyComponentManager = new CEnemyComponentManager(*myScene);
	myMovementComponentManager = new CMovementComponentManager();

	myAmmoComponentManager = new CAmmoComponentManager();
	myWeaponFactory = new CWeaponFactory();
	myWeaponFactory->Init(myGameObjectManager, myModelComponentManager);
	myWeaponSystemManager = new CWeaponSystemManager(myWeaponFactory);
	myProjectileComponentManager = new CProjectileComponentManager();
	myProjectileFactory = new CProjectileFactory(myProjectileComponentManager);
	myProjectileFactory->Init(myGameObjectManager, myModelComponentManager);
}

void CPlayState::SpawnOtherPlayer(unsigned aPlayerID)
{
	CGameObject* otherPlayer = myGameObjectManager->CreateGameObject();
	CModelComponent* model = myModelComponentManager->CreateComponent("Models/chromeBall/chromeBall.fbx");
	CNetworkPlayerReciverComponent* playerReciver = new CNetworkPlayerReciverComponent;
	playerReciver->SetPlayerID(aPlayerID);
	CComponentManager::GetInstance().RegisterComponent(playerReciver);
	otherPlayer->AddComponent(model);
	otherPlayer->AddComponent(playerReciver);

	

	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new COtherPlayerSpawned(playerReciver));
}

void CPlayState::TempHardCodePlayerRemoveTHisLaterWhenItIsntNecessaryToHaveAnymore(CU::Camera& aCamera)
{
	//create hard coded player:
	{
		CCameraComponent* cameraComponent = myCameraComponent;
		if (cameraComponent == nullptr)
		{
			DL_PRINT_WARNING("No camera found in scene, creating default at height 1.80m at position (0, 0, 0)");
			cameraComponent = new CCameraComponent();
			CComponentManager::GetInstance().RegisterComponent(cameraComponent);
			cameraComponent->SetCamera(aCamera);

			CGameObject* cameraObject = myGameObjectManager->CreateGameObject();
			cameraObject->GetLocalTransform().SetPosition(0.f, 1.8f, 0.f);
			cameraObject->AddComponent(cameraComponent);
		}

		CGameObject* playerObject = cameraComponent->GetParent()->GetParent();
		if (playerObject == nullptr)
		{
			playerObject = myGameObjectManager->CreateGameObject();
			playerObject->GetLocalTransform().SetPosition(0, 0, 0);
		}

		playerObject->AddComponent(cameraComponent->GetParent());

		CInputComponent* inputComponent = new CInputComponent();
		CComponentManager::GetInstance().RegisterComponent(inputComponent);
		playerObject->AddComponent(inputComponent);

		myMovementComponent = myMovementComponentManager->CreateAndRegisterComponent();
		playerObject->AddComponent(myMovementComponent);

		CWeaponSystemComponent* weaponSystenComponent = myWeaponSystemManager->CreateAndRegisterComponent();
		CAmmoComponent* ammoComponent = myAmmoComponentManager->CreateAndRegisterComponent();
		playerObject->AddComponent(weaponSystenComponent);
		playerObject->AddComponent(ammoComponent);
		SComponentMessageData addHandGunData;
		SComponentMessageData giveAmmoData;

		addHandGunData.myString = "Handgun";
		playerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addHandGunData);
		SAmmoReplenishData tempAmmoReplensihData;
		tempAmmoReplensihData.ammoType = "Handgun";
		tempAmmoReplensihData.replenishAmount = 100;
		giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		playerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

		addHandGunData.myString = "Shotgun";
		playerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addHandGunData);
		tempAmmoReplensihData.ammoType = "Shotgun";
		tempAmmoReplensihData.replenishAmount = 100;
		giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		playerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

		addHandGunData.myString = "PlasmaRifle";
		playerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addHandGunData);
		tempAmmoReplensihData.ammoType = "PlasmaRifle";
		tempAmmoReplensihData.replenishAmount = 1000;
		giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		playerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

		CPlayerNetworkComponent* network = new CPlayerNetworkComponent();
		CComponentManager::GetInstance().RegisterComponent(network);

		playerObject->AddComponent(network);

		Component::CEnemy::SetPlayer(playerObject);
	}
}
