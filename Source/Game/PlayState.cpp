#include "stdafx.h"
#include "PlayState.h"

#include <Engine.h>
#include <CommonUtilities.h>
#include <StopWatch.h>
#include <EInputReturn.h>
#include <Lights.h>
#include <TimerManager.h>
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
//#include "../GUI/GUIManager.h"

#include "LoadManager/LoadManager.h"
#include "KevinLoader/KevinLoader.h"

#include "CameraComponent.h"
#include "InputComponent.h"
#include <chrono>
#include <thread>
#include "MovementComponent.h"
#include "ModelComponentManager.h"
#include "EnemyComponentManager.h"
#include "Enemy.h"

#include "WeaponSystemComponent.h"
#include "AmmoComponent.h"
#include "ComponentMessage.h"
#include "InputComponentManager.h"
#include "TClient/Client.h"
#include "TShared/NetworkMessage_ClientReady.h"
#include "TClient/ClientMessageManager.h"

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

	CComponentManager::DestroyInstance();
}

void CPlayState::Load()
{
	CU::TimerManager timerMgr;
	CU::TimerHandle handle = timerMgr.CreateTimer();
	timerMgr.StartTimer(handle);
	CU::CStopWatch loadPlaystateTimer;

	srand(static_cast<unsigned int>(time(nullptr)));

	LoadManagerGuard loadManagerGuard(*this, *myScene);
	CreateManagersAndFactories();

	Lights::SDirectionalLight dirLight;
	dirLight.color = { .25f, .25f, .25f, 1.0f };
	dirLight.direction = { -1.0f, -1.0f, 1.0f, 1.0f };
	myScene->AddDirectionalLight(dirLight);


	myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	CU::Camera& playerCamera = myScene->GetCamera(CScene::eCameraType::ePlayerOneCamera);
	playerCamera.Init(90, WINDOW_SIZE_F.x, WINDOW_SIZE_F.y, 0.1f, 1000.f);
	
	myWeaponFactory->LoadWeapons();

	//create player:
	{
		CCameraComponent* cameraComponent = new CCameraComponent();
		CComponentManager::GetInstance().RegisterComponent(cameraComponent);
		cameraComponent->SetCamera(playerCamera);
		CGameObject* playerObject = myGameObjectManager->CreateGameObject();
		playerObject->GetLocalTransform().SetPosition(0, 0, 0);
		Component::CEnemy::SetPlayer(playerObject);
		CGameObject* cameraObject = myGameObjectManager->CreateGameObject();
		cameraObject->GetLocalTransform().SetPosition(0.f, 180.f, 0.f); //ändrat till cm igen får ses över //Alex
		cameraObject->AddComponent(cameraComponent);
		playerObject->AddComponent(cameraObject);

		CInputComponent* inputComponent = myInputComponentManager->CreateAndRegisterComponent();
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
		giveAmmoData.myInt = 100;
		playerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);
	}
	

	//myGameObjectManager->SendObjectsDoneMessage();

	myScene->SetSkybox("default_cubemap.dds");
	
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

	std::string levelPath = "Json/Levels/";
	levelPath += levelsArray[myLevelIndex].GetString();
	levelPath += "/LevelData.json";

	const KLoader::eError loadError = loader.LoadFile(levelPath);
	if (loadError != KLoader::eError::NO_LOADER_ERROR)
	{
		DL_MESSAGE_BOX("Loading Failed");
	}
	myIsLoaded = true;

	//skicka meddelande om att laddning e klar.
	/*SNetworkPackageHeader header;
	header.myTargetID = (short)ePreDefId::ID_SERVER;
	header.myPackageType = (char)ePackageType::eClientReady;
	CClient::GetInstance().myMessageManager->
	CClient::GetInstance().Send()*/

	// Get time to load the level:
	timerMgr.UpdateTimers();
	float time = timerMgr.GetTimer(handle).GetLifeTime().GetMilliseconds();
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
	myAmmoComponentManager->Update(aDeltaTime);
	myInputComponentManager->Update();

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

	myScene = new CScene();

	//LoadManager::GetInstance().SetCurrentPlayState(this);
	//LoadManager::GetInstance().SetCurrentScene(myScene);


	myGameObjectManager = new CGameObjectManager();
	myModelComponentManager = new CModelComponentManager(*myScene);
	myEnemyComponentManager = new CEnemyComponentManager(*myScene);

	myAmmoComponentManager = new CAmmoComponentManager();
	myWeaponFactory = new CWeaponFactory();
	myWeaponSystemManager = new CWeaponSystemManager(myWeaponFactory);
	myProjectileComponentManager = new CProjectileComponentManager();
	myProjectileFactory = new CProjectileFactory(myProjectileComponentManager);
	myProjectileFactory->Init(myGameObjectManager, myModelComponentManager);
	myInputComponentManager = new CInputComponentManager();
}
