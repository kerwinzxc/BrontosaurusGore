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

CPlayState::CPlayState(StateStack& aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::ePlayState, 1)
	, myLevelIndex(aLevelIndex)
	, myGameObjectManager(nullptr)
	, myScene(nullptr)
	, myModelComponentManager(nullptr)
	, myMovementComponent(nullptr)
	, myAmmoComponentManager(nullptr)
	, myWeaponFactory(nullptr)
	, myWeaponSystemManager(nullptr)
	, myProjectileComponentManager(nullptr)
	, myProjectileFactory(nullptr)
	, myInputComponentManager(nullptr)
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

	CNetworkComponentManager::Destroy();

	CComponentManager::DestroyInstance();
}

void CPlayState::Load()
{
	CU::CStopWatch loadPlaystateTimer;
	loadPlaystateTimer.Start();

	srand(static_cast<unsigned int>(time(nullptr)));

	CreateManagersAndFactories();
	LoadManagerGuard loadManagerGuard(*this, *myScene);

	Lights::SDirectionalLight dirLight;
	dirLight.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	dirLight.direction = { -1.0f, -1.0f, 1.0f, 1.0f };
	myScene->AddDirectionalLight(dirLight);


	myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	CU::Camera& playerCamera = myScene->GetCamera(CScene::eCameraType::ePlayerOneCamera);
	playerCamera.Init(90, WINDOW_SIZE_F.x, WINDOW_SIZE_F.y, 0.1f, 1000.f);
	
	myWeaponFactory->LoadWeapons();




	//real loading:		as opposed to fake loading
	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();
	CU::CJsonValue levelsFile;

	std::string errorString = levelsFile.Parse("Json/LevelList.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	CU::CJsonValue levelsArray = levelsFile.at("levels");



	std::string levelPath = "Json/Levels/";
	levelPath += levelsArray[myLevelIndex].GetString();
	levelPath += "/LevelData.json";

	const KLoader::eError loadError = loader.LoadFile(levelPath);
	if (loadError != KLoader::eError::NO_LOADER_ERROR)
	{
		DL_MESSAGE_BOX("Loading Failed");
	}



	//create hard coded player:
	{
		CCameraComponent* cameraComponent = new CCameraComponent();
		CComponentManager::GetInstance().RegisterComponent(cameraComponent);
		cameraComponent->SetCamera(playerCamera);
		CGameObject* playerObject = myGameObjectManager->CreateGameObject();
		playerObject->GetLocalTransform().SetPosition(0, 0, 0);
		Component::CEnemy::SetPlayer(playerObject);
		CGameObject* cameraObject = myGameObjectManager->CreateGameObject();
		cameraObject->GetLocalTransform().SetPosition(0.f, 1.8f, 0.f);
		cameraObject->AddComponent(cameraComponent);
		playerObject->AddComponent(cameraObject);

		CInputComponent* inputComponent = new CInputComponent();
		CComponentManager::GetInstance().RegisterComponent(inputComponent);
		playerObject->AddComponent(inputComponent);

		myMovementComponent = new CMovementComponent();
		playerObject->AddComponent(myMovementComponent);
		CWeaponSystemComponent* weaponSystenComponent = myWeaponSystemManager->CreateAndRegisterComponent();
		CAmmoComponent* ammoComponent = myAmmoComponentManager->CreateAndRegisterComponent();
		playerObject->AddComponent(weaponSystenComponent);
		playerObject->AddComponent(ammoComponent);
		SComponentMessageData addHandGunData;
		SComponentMessageData giveAmmoData;
		addHandGunData.myString = "Handgun";
		playerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addHandGunData);
		playerObject->NotifyOnlyComponents(eComponentMessageType::eChangeSelectedAmmoType, addHandGunData);
		giveAmmoData.myInt = 100;
		playerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);
		addHandGunData.myString = "Shotgun";
		playerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addHandGunData);
		playerObject->NotifyOnlyComponents(eComponentMessageType::eChangeSelectedAmmoType, addHandGunData);
		giveAmmoData.myInt = 100;
		playerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);
		addHandGunData.myString = "PlasmaRifle";
		playerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addHandGunData);
		playerObject->NotifyOnlyComponents(eComponentMessageType::eChangeSelectedAmmoType, addHandGunData);
		giveAmmoData.myInt = 1000;
		playerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);
	}
	

	//myGameObjectManager->SendObjectsDoneMessage();

	myScene->SetSkybox("default_cubemap.dds");
	myScene->SetCubemap("purpleCubemap.dds");
	
	myIsLoaded = true;
	
	// Get time to load the level:
	loadPlaystateTimer.Update();
	float time = loadPlaystateTimer.GetDeltaTime().GetMilliseconds();
	GAMEPLAY_LOG("Game Inited in %f ms", time);
}

void CPlayState::Init()
{
}

eStateStatus CPlayState::Update(const CU::Time& aDeltaTime)
{
	myMovementComponent->Update(aDeltaTime);
	myEnemyComponentManager->Update(aDeltaTime);
	myScene->Update(aDeltaTime);
	myWeaponSystemManager->Update(aDeltaTime);
	myProjectileComponentManager->Update(aDeltaTime);
	myProjectileFactory->Update(aDeltaTime.GetSeconds());
	myAmmoComponentManager->Update(aDeltaTime);

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
}

eMessageReturn CPlayState::Recieve(const Message& aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

CU::eInputReturn CPlayState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	CU::CInputMessenger::RecieveInput(aInputMessage);
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

	//LoadManager::GetInstance().SetCurrentPlayState(this);
	//LoadManager::GetInstance().SetCurrentScene(myScene);


	myGameObjectManager = new CGameObjectManager();
	myModelComponentManager = new CModelComponentManager(*myScene);
	myEnemyComponentManager = new CEnemyComponentManager(*myScene);

	myAmmoComponentManager = new CAmmoComponentManager();
	myWeaponFactory = new CWeaponFactory();
	myWeaponFactory->Init(myGameObjectManager, myModelComponentManager);
	myWeaponSystemManager = new CWeaponSystemManager(myWeaponFactory);
	myProjectileComponentManager = new CProjectileComponentManager();
	myProjectileFactory = new CProjectileFactory(myProjectileComponentManager);
	myProjectileFactory->Init(myGameObjectManager, myModelComponentManager);
}
