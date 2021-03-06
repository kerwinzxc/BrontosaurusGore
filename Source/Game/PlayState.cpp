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

#include "LoadState.h"
#include "ThreadedPostmaster/LoadLevelMessage.h"
#include "PollingStation.h"

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
#include "Components/ScriptComponentManager.h"
#include "Components/PickupComponentManager.h"
#include "Components/ExplosionFactory.h"
#include "Components/ExplosionComponentManager.h"
#include "Components/HealthComponentManager.h"
#include "DamageOnCollisionComponentManager.h"
#include "Components/DoorManager.h"
#include "../Components/CheckpointComponentManager.h"
#include "EnemyFactory.h"
#include "TumbleweedFactory.h"
#include "Components/MarkerComponentManager.h"
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
#include "../Components/CheckPointSystem.h"

#include "../Components/PlayerNetworkComponent.h"

//Hard code necessary includes
#include "AmmoReplenishData.h"
#include "ThreadedPostmaster/OtherPlayerSpawned.h"
#include "HealthComponent.h"
#include "CheckPointComponent.h"
#include "Enemy.h"
//


//PHYSICS
#include "../Physics/Foundation.h"
#include "../Physics/Physics.h"
#include "../Physics/PhysicsScene.h"
#include "../Physics/PhysicsActorDynamic.h"
#include "ColliderComponentManager.h"
#include "BoxColliderComponent.h"
#include "Physics/PhysicsCharacterController.h"
#include "CharacterControllerComponent.h"
#include "../Components/ParticleEmitterComponentManager.h"
#include "EnemyClientRepresentationManager.h"

#include "TextInstance.h"
#include "GameObject.h"
#include "ComponentAnswer.h"
#include "AnimationComponent.h"
#include "LutComponent.h"
#include "PointLightComponentManager.h"
#include "BrontosaurusEngine/SpriteInstance.h"
#include "ShowTitleComponent.h"
#include "Renderer.h"
#include "MenuState.h"
#include "ThreadedPostmaster/GameEventMessage.h"
#include <LuaWrapper/SSlua/SSlua.h>

CPlayState::CPlayState(StateStack& aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::ePlayState, 1)
	, myGameObjectManager(nullptr)
	, myScene(nullptr)
	, myModelComponentManager(nullptr)
	, myAmmoComponentManager(nullptr)
	, myWeaponSystemManager(nullptr)
	, myWeaponFactory(nullptr)
	, myProjectileComponentManager(nullptr)
	, myProjectileFactory(nullptr)
	, myInputComponentManager(nullptr)
	, myMovementComponentManager(nullptr)
	, myScriptComponentManager(nullptr)
	, myExplosionFactory(nullptr)
	, myExplosionComponentManager(nullptr)
	, myMovementComponent(nullptr)
	, myCameraComponent(nullptr)
	, myIsInfocus(false)
	, myLevelToSwapTo(-1)
	, myLevelIndex(aLevelIndex)
	, myIsLoaded(false)
	, myIsCutscene(false)
	, myPressedAnyKey(false)
	, myPressedEscDuringCutscene(false)
	, myPressEscTimer(0.f)
	, mode(0)
{
	myPhysicsScene = nullptr;
	myPhysics = nullptr;
	myColliderComponentManager = nullptr;
	myCheckPointSystem = nullptr;
	new CPollingStation();
}

CPlayState::~CPlayState()
{
	CParticleEmitterComponentManager::Destroy();
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);

	SAFE_DELETE(myScene);

	SAFE_DELETE(myModelComponentManager);

	SAFE_DELETE(myAmmoComponentManager);
	SAFE_DELETE(myWeaponFactory);
	SAFE_DELETE(myProjectileComponentManager);
	SAFE_DELETE(myProjectileFactory);
	SAFE_DELETE(myMovementComponentManager);
	SAFE_DELETE(myEnemyComponentManager);
	SAFE_DELETE(myScriptComponentManager);
	SAFE_DELETE(myExplosionFactory);
	SAFE_DELETE(myExplosionComponentManager);
	SAFE_DELETE(myDamageOnCollisionComponentManager);

	CPointLightComponentManager::Destroy();
	CDoorManager::Destroy();
	CNetworkComponentManager::Destroy();
	CCheckpointComponentManager::DestoryInstance();
	CComponentManager::DestroyInstance();
	CPickupComponentManager::Destroy();
	CEnemyClientRepresentationManager::Destroy();
	CHealthComponentManager::Destroy();
	CEnemyFactory::Destroy();
	SAFE_DELETE(myColliderComponentManager);
	SAFE_DELETE(myPhysicsScene);
	SAFE_DELETE(myGameObjectManager);
	CTumbleweedFactory::DestroyInstance();
	CMarkerComponentManager::DestroyInstance();
	//SAFE_DELETE(myPhysics); // kanske? nope foundation f�rst�r den
	//Physics::CFoundation::Destroy(); desstroy this lator

	delete CPollingStation::GetInstance();
}

void CPlayState::Load()
{
	CU::CStopWatch loadPlaystateTimer;
	loadPlaystateTimer.Start();

	srand(static_cast<unsigned int>(time(nullptr)));

	CU::CJsonValue levelsFile;
	std::string errorString = levelsFile.Parse("Json/LevelList.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	//**************************************************************//
	//							PHYSICS								//
	//**************************************************************//
	if (Physics::CFoundation::GetInstance() == nullptr)
	{
		Physics::CFoundation::Create();
		if (Physics::CFoundation::GetInstance()->GetPhysics() == nullptr)
		{
			myPhysics = Physics::CFoundation::GetInstance()->CreatePhysics();
		}
	}
	else
	{
		myPhysics = Physics::CFoundation::GetInstance()->GetPhysics();
	}

	if (myPhysicsScene != nullptr)
	{
		delete myPhysicsScene;
	}

	myPhysicsScene = myPhysics->CreateScene();

	//**************************************************************//
	//						END OF PHYSICS							//
	//**************************************************************//
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

	//real loading:		as opposed to fake loading
	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();

	const KLoader::eError loadError = loader.LoadFile(levelPath);
	if (loadError != KLoader::eError::NO_LOADER_ERROR)
	{
		DL_MESSAGE_BOX("Loading Failed");
	}

	if (levelsArray[myLevelIndex].GetString() == "Intro")
	{
		myTitle.Init(3);

		myIsCutscene = true;
		mode = 1;
		Audio::CAudioInterface::GetInstance()->PostEvent("Intro_Audio");
		myTitle.Add(nullptr);

		myTitle.Add(new CSpriteInstance("Sprites/Splash/timedust.dds"));
		myTitle.GetLast()->SetPivot({ 0.5f, 0.5f });
		myTitle.GetLast()->SetPosition({ 0.5f, 0.5f });


		myTitle.Add(new CSpriteInstance("Sprites/Menu/Main/Title/default.dds"));
		myTitle.GetLast()->SetPivot({ 0.5f, 0.5f });
		myTitle.GetLast()->SetPosition({ 0.5f, 0.5f });
	}
	else if (levelsArray[myLevelIndex].GetString() == "Outro")
	{
		myTitle.Init(3);
		myIsCutscene = true;
		mode = 2;
		myTitle.Add(new CSpriteInstance("Sprites/Splash/credits.dds"));
		myTitle.GetLast()->SetPivot({ 0.5f, 0.5f });
		myTitle.GetLast()->SetPosition({ 0.5f, 0.5f });


		myTitle.Add(new CSpriteInstance("Sprites/Splash/timedust.dds"));
		myTitle.GetLast()->SetPivot({ 0.5f, 0.5f });
		myTitle.GetLast()->SetPosition({ 0.5f, 0.5f });


		Audio::CAudioInterface::GetInstance()->PostEvent("Outro_Audio");
	}

	Lights::SDirectionalLight dirLight;
	dirLight.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	dirLight.direction = { -1.0f, -1.0f, 1.0f, 1.0f };
	dirLight.shadowIndex = 0;
	myScene->AddDirectionalLight(dirLight);
	myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	CRenderCamera& playerCamera = myScene->GetRenderCamera(CScene::eCameraType::ePlayerOneCamera);
	playerCamera.InitPerspective(90, WINDOW_SIZE_F.x, WINDOW_SIZE_F.y, 0.1f, myIsCutscene ? 2000.f : 500.f);

	if (myIsCutscene == false)
	{
		myWeaponFactory->LoadWeapons();

	}

	CreatePlayer(playerCamera.GetCamera()); // Hard codes Player!;


	myScene->SetSkybox("default_cubemap.dds");
	myScene->SetCubemap("purpleCubemap.dds");

	if (myIsCutscene == false)
	{
		myHUD.LoadHUD();
	}

	myIsLoaded = true;

	CEnemyFactory::GetInstance()->LoadBluePrints(levelsArray.at(myLevelIndex).GetString());

	// Get time to load the level:
	loadPlaystateTimer.Update();
	float time = loadPlaystateTimer.GetDeltaTime().GetMilliseconds();
	GAMEPLAY_LOG("Game Inited in %f ms", time);
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();

	if (!myIsCutscene)
	{
		Audio::CAudioInterface::GetInstance()->PostEvent("Player_Chainsaw_Throttle_Start"); // g�r possitionerat.
	}
}

void CPlayState::Init()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eChangeLevel);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);

	myCheckPointSystem = new CCheckPointSystem();
	myGameObjectManager->SendObjectsDoneMessage();
	myExplosionFactory->Init(myGameObjectManager, myModelComponentManager, myColliderComponentManager);
	CEnemyClientRepresentationManager::GetInstance().Init(myWeaponSystemManager);
}

eStateStatus CPlayState::Update(const CU::Time& aDeltaTime)
{
	if (myLevelToSwapTo > -1)
	{
		myStateStack.SwapState(new CLoadState(myStateStack, myLevelToSwapTo));
	}

	myMovementComponentManager->Update(aDeltaTime);
	myEnemyComponentManager->Update(aDeltaTime.GetSeconds());
	myWeaponSystemManager->Update(aDeltaTime);
	myProjectileComponentManager->Update(aDeltaTime);
	myProjectileFactory->Update(aDeltaTime.GetSeconds());
	myAmmoComponentManager->Update(aDeltaTime);
	CParticleEmitterComponentManager::GetInstance().UpdateEmitters(aDeltaTime);
	myExplosionComponentManager->Update(aDeltaTime);
	myDamageOnCollisionComponentManager->Update(aDeltaTime);
	CEnemyClientRepresentationManager::GetInstance().Update(aDeltaTime);
	CPickupComponentManager::GetInstance()->Update(aDeltaTime.GetSeconds());
	if(CMarkerComponentManager::GetInstance() != nullptr)
	{
	
		CMarkerComponentManager::GetInstance()->Update(aDeltaTime.GetSeconds());
	}
	CDoorManager::GetInstance()->Update(aDeltaTime);

	if (myIsCutscene == false)
	{
		myHUD.Update(aDeltaTime);
	}

	CTumbleweedFactory::GetInstance()->Update(aDeltaTime.GetSeconds());

	CAnimationComponent::UpdateAnimations(aDeltaTime);
	myScene->Update(aDeltaTime);
	if (myPhysicsScene->Simulate(aDeltaTime) == true)
	{
		myColliderComponentManager->Update();
	}
	myPlayerLut->Update(aDeltaTime.GetSeconds());

	if (myIsCutscene)
	{
		if (myTitleComponent)
		{
			if (myPressEscTimer >= 0.f)
			{
				myPressEscTimer -= aDeltaTime.GetSeconds();
				if (myPressedEscDuringCutscene)
				{
					if (mode == 1)
					{
						//change level
						SSlua::LuaWrapper::GetInstance().DoString("ChangeLevel(0)");
					}
					else if (mode == 2)
					{
						//go to credits and stuff
					}
				}
			}

			if (myPressedAnyKey && myPressEscTimer <= 0.f)
			{
				myPressedAnyKey = false/*!true*/;
				myPressEscTimer = 2.f;

				CGameEventMessage* pressToSkipMessage = new CGameEventMessage(L"Press ESC to skip cut scene");
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(pressToSkipMessage);
			}

			myTitleComponent->Update(aDeltaTime.GetSeconds());
			if (myTitleComponent->GetIndex() < myTitle.Size())
			{
				if (myTitle[myTitleComponent->GetIndex()])
				{
					myTitle[myTitleComponent->GetIndex()]->SetAlpha(myTitleComponent->GetAlpha());
				}
			}
		}
	}

	myPressedEscDuringCutscene = false;

	return myStatus;
}

void CPlayState::Render()
{
	myScene->Render();

	if (myIsCutscene == false)
	{
		myHUD.Render();
	}

	if (myIsCutscene == true)
	{
		if (myTitleComponent->GetIndex() < myTitle.Size())
		{
			if (myTitle[myTitleComponent->GetIndex()])
			{
				myTitle[myTitleComponent->GetIndex()]->Render();
			}
		}
	}

}

void CPlayState::OnEnter(const bool /*aLetThroughRender*/)
{
	myIsInfocus = true;

}

void CPlayState::OnExit(const bool /*aLetThroughRender*/)
{
	myIsInfocus = false;

	RENDERER.ClearGui();
}

void CPlayState::Pause()
{
	myStateStack.PushState(new CMenuState(myStateStack, "Json/Menu/PauseMenu.json"));
}

CU::eInputReturn CPlayState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	if (myIsInfocus == false)
	{
		return CU::eInputReturn::ePassOn;
	}
	if (aInputMessage.myType == CU::eInputType::eKeyboardPressed)
	{
		myPressedAnyKey = true;
	}

	if (aInputMessage.myType == CU::eInputType::eKeyboardPressed && aInputMessage.myKey == CU::eKeys::ESCAPE)
	{
		if (myPressEscTimer >= 0.f && myIsCutscene)
		{
			myPressedEscDuringCutscene = true;
		}
		else if (!myIsCutscene)
		{
			Pause();
		}
	}
	else
	{
		CU::CInputMessenger::RecieveInput(aInputMessage);
	}

	return	 CU::eInputReturn::eKeepSecret;
}

CGameObjectManager* CPlayState::GetGameObjectManager()
{
	return myGameObjectManager;
}

eMessageReturn CPlayState::DoEvent(const CLoadLevelMessage& aLoadLevelMessage)
{
	myLevelToSwapTo = aLoadLevelMessage.myLevelIndex;
	return eMessageReturn::eContinue;
}

void CPlayState::CreateManagersAndFactories()
{
	CComponentManager::CreateInstance();

	CCheckpointComponentManager::CreateInstance();
	CNetworkComponentManager::Create();
	CHealthComponentManager::Create();
	CDoorManager::Create();
	CParticleEmitterComponentManager::Create();

	myScene = new CScene();

	CPointLightComponentManager::Create(*myScene);
	myGameObjectManager = new CGameObjectManager();
	myModelComponentManager = new CModelComponentManager(*myScene);
	myEnemyComponentManager = new CEnemyComponentManager();
	myMovementComponentManager = new CMovementComponentManager();

	myColliderComponentManager = new CColliderComponentManager();
	myColliderComponentManager->SetPhysicsScene(myPhysicsScene);
	myColliderComponentManager->SetPhysics(myPhysics);
	myColliderComponentManager->InitControllerManager();



	myAmmoComponentManager = new CAmmoComponentManager();
	myWeaponFactory = new CWeaponFactory();
	myWeaponFactory->Init(myGameObjectManager, myModelComponentManager, myPhysicsScene);
	myWeaponSystemManager = new CWeaponSystemManager(myWeaponFactory);
	myProjectileComponentManager = new CProjectileComponentManager();
	myProjectileFactory = new CProjectileFactory(myProjectileComponentManager);
	myProjectileFactory->Init(myGameObjectManager, myModelComponentManager, myColliderComponentManager);
	myDamageOnCollisionComponentManager = new CDamageOnCollisionComponentManager();

	myScriptComponentManager = new CScriptComponentManager();
	CPickupComponentManager::Create();
	CEnemyClientRepresentationManager::Create();
	CEnemyFactory::Create(*myEnemyComponentManager, *myGameObjectManager, *myWeaponSystemManager, *myColliderComponentManager);

	myExplosionComponentManager = new CExplosionComponentManager();
	myExplosionFactory = new CExplosionFactory(myExplosionComponentManager);
	CTumbleweedFactory::CreateInstance();
	CTumbleweedFactory::GetInstance()->Init(myGameObjectManager, myColliderComponentManager);
	CMarkerComponentManager::CreateInstance();
}

void CPlayState::SpawnOtherPlayer(unsigned aPlayerID)
{
	CGameObject* otherPlayer = myGameObjectManager->CreateGameObject();
	CGameObject* modelObject = myGameObjectManager->CreateGameObject();
	CU::Matrix44f transformation;
	transformation.RotateAroundAxis(3.14, CU::Axees::Y);
	transformation.SetPosition(0, -1.8, 0);
	modelObject->SetWorldTransformation(transformation);

	CModelComponent* model = myModelComponentManager->CreateComponent("Models/Animations/M_Player_01.fbx");
	CModelComponent* weaponModel = myModelComponentManager->CreateComponent("Models/Animations/M_ShotgunPlayerAnim_01.fbx");
	CNetworkPlayerReciverComponent* playerReciver = new CNetworkPlayerReciverComponent;
	playerReciver->SetPlayerID(aPlayerID);
	CComponentManager::GetInstance().RegisterComponent(playerReciver);

	CWeaponSystemComponent* weaponSystenComponent = myWeaponSystemManager->CreateAndRegisterComponent();
	CAmmoComponent* ammoComponent = myAmmoComponentManager->CreateAndRegisterComponent();
	otherPlayer->AddComponent(weaponSystenComponent);
	otherPlayer->AddComponent(ammoComponent);
	SComponentMessageData addHandGunData;
	SComponentMessageData giveAmmoData;

	SAmmoReplenishData tempAmmoReplensihData;

	addHandGunData.myString = "MeleeWeapon";
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
	tempAmmoReplensihData.ammoType = "MeleeWeapon";
	tempAmmoReplensihData.replenishAmount = 1000000;
	giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

	addHandGunData.myString = "PlasmaRifle";
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
	tempAmmoReplensihData.ammoType = "PlasmaRifle";
	tempAmmoReplensihData.replenishAmount = 1000;
	giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

	addHandGunData.myString = "Shotgun";
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
	tempAmmoReplensihData.ammoType = "Shotgun";
	tempAmmoReplensihData.replenishAmount = 100;
	giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

	addHandGunData.myString = "BFG";
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
	tempAmmoReplensihData.ammoType = "BFG";
	tempAmmoReplensihData.replenishAmount = 100;
	giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

	modelObject->AddComponent(model);
	modelObject->AddComponent(weaponModel);
	otherPlayer->AddComponent(modelObject);
	otherPlayer->AddComponent(playerReciver);
	CPollingStation::GetInstance()->AddPlayerObject(otherPlayer);


	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new COtherPlayerSpawned(playerReciver));
}

void CPlayState::CreatePlayer(CU::Camera& aCamera)
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
			playerObject->AddComponent(cameraComponent->GetParent());
		}

		if (CPollingStation::GetInstance())
		{
			CPollingStation::GetInstance()->SetPlayerObject(playerObject);
			CPollingStation::GetInstance()->AddPlayerObject(playerObject);
		}


		if (myIsCutscene == false)
		{
			CInputComponent* inputComponent = new CInputComponent();
			CComponentManager::GetInstance().RegisterComponent(inputComponent);
			playerObject->AddComponent(inputComponent);
		}

		myMovementComponent = myMovementComponentManager->CreateAndRegisterComponent();
		playerObject->AddComponent(myMovementComponent);
		if (myIsCutscene == true)
		{
			myMovementComponent->SetIntroFallMode( mode == 1 ? -27.f : -10.f);
			myTitleComponent = new CShowTitleComponent();
			playerObject->AddComponent(myTitleComponent);
		}

		if (myIsCutscene == false)
		{
			CWeaponSystemComponent* weaponSystenComponent = myWeaponSystemManager->CreateAndRegisterComponent();
			CAmmoComponent* ammoComponent = myAmmoComponentManager->CreateAndRegisterComponent();
			playerObject->AddComponent(weaponSystenComponent);
			playerObject->AddComponent(ammoComponent);
			GivePlayerWeapons(playerObject);

		}

		myPlayerLut = new CLutComponent();
		playerObject->AddComponent(myPlayerLut);


		//addHandGunData.myString = "MeleeWeapon";
		//playerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
		//tempAmmoReplensihData.ammoType = "MeleeWeapon";
		//tempAmmoReplensihData.replenishAmount = 1000000;
		//giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		//playerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

		CPlayerNetworkComponent* network = new CPlayerNetworkComponent();
		CComponentManager::GetInstance().RegisterComponent(network);

		playerObject->AddComponent(network);

		Physics::SCharacterControllerDesc controllerDesc;
		controllerDesc.minMoveDistance = 0.00001f;
		controllerDesc.halfHeight = 1.f;
		controllerDesc.radius = 0.5f;
		controllerDesc.halfHeight = 2.f;
		controllerDesc.center.y = -3.0f;

		float rad = 45.f;
		DEGREES_TO_RADIANS(rad);
		controllerDesc.slopeLimit = rad;
		controllerDesc.stepOffset = 1.0f;
		CCharacterControllerComponent* controller = myColliderComponentManager->CreateCharacterControllerComponent(controllerDesc, playerObject->GetId());
		playerObject->AddComponent(controller);

		CHealthComponent* playerHealthComponent = new CHealthComponent(99999);
		CU::CJsonValue playerControls;
		std::string errorMessage = playerControls.Parse("Json/Player/playerData.json");
		if (!errorMessage.empty())
		{
			DL_PRINT_WARNING("Could not load %s, using default values", errorMessage.c_str());

			playerHealthComponent->SetMaxHealth(200);
			playerHealthComponent->SetHealth(200);
			playerHealthComponent->SetMaxArmor(200);
		}
		else
		{

			playerHealthComponent->SetMaxHealth(static_cast<healthPoint>(playerControls["MaxHealth"].GetFloat()));
			playerHealthComponent->SetHealth(static_cast<healthPoint>(playerControls["MaxHealth"].GetFloat()));
			playerHealthComponent->SetMaxArmor(static_cast<healthPoint>(playerControls["MaxArmor"].GetFloat()));
		}
		playerHealthComponent->SetArmor(0);

		playerObject->AddComponent(playerHealthComponent);
		//CTumbleweedFactory::GetInstance()->CreateNewTumbleweed(playerObject->GetWorldPosition());

		/*SBoxColliderData data;
		data.myHalfExtent = CU::Vector3f(0.25, 0.9, 0.25);
		data.center = CU::Vector3f(0.f, 0.9f, 0.f);
		data.myType = SColliderData::eColliderType::eBox;
		data.material = CU::Vector3f(0, 1, 0);
		data.IsTrigger = false;
		CColliderComponent* collider = myColliderComponentManager->CreateComponent(&data ,playerObject->GetId());*/



		//CGameObject* enemyObject = myGameObjectManager->CreateGameObject();
		//CModelComponent* enemyModelComponent = myModelComponentManager->CreateComponent("Models/Meshes/M_Enemy_DollarDragon_01.fbx");
		//enemyObject->AddComponent(enemyModelComponent);
		//
		//CEnemyComponentManager::EnemyBlueprint bluePrint;
		//bluePrint.health = 10;
		//bluePrint.detactionRange = 1000000;
		//bluePrint.startAttackRange = 0;
		//bluePrint.stopAttackRange = 0;
		//bluePrint.speed = 0;
		//CEnemy* enemyEnemy = myEnemyComponentManager->CreateComponent(bluePrint, 1000000);
		//enemyObject->AddComponent(enemyEnemy);
		//enemyObject->SetWorldPosition(CU::Vector3f(-5, -33, -13));

			/*SSphereColliderData sphereColliderData;
			sphereColliderData.IsTrigger = false;
			sphereColliderData.myRadius = 0.5f;
			CColliderComponent* enemySphereColiider = myColliderComponentManager->CreateComponent(&sphereColliderData);
			enemyObject->AddComponent(enemySphereColiider);*/
			/*CCheckPointComponent* enemyRespanwsPlayerLol = new CCheckPointComponent();
			enemyRespanwsPlayerLol->SetCheckPointPosition(CU::Vector3f::Zero);
			enemyObject->AddComponent(enemyRespanwsPlayerLol);
			enemyObject->SetWorldPosition(CU::Vector3f(0.0f, 3.0f, 0.0f));
			enemyObject->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());*/

	}
}

void CPlayState::GivePlayerWeapons(CGameObject* aPlayerObject)
{
	CU::CJsonValue levelsFile;

	std::string errorString = levelsFile.Parse("Json/LevelList.json");

	CU::CJsonValue levelsArray = levelsFile.at("levels");


	const std::string & alevel = levelsArray.at(myLevelIndex).GetString();
	CU::CJsonValue value("Json/Weapons/PlayerStartWeapons.json");

	SComponentMessageData addHandGunData;
	SComponentMessageData giveAmmoData;
	SAmmoReplenishData tempAmmoReplensihData;

	if (value.Count(alevel) <= 0)
	{
		DL_PRINT_WARNING("Didn't the level %s in PlayerStartWeapons.json, giving all weapons to player as default, if you need help ask Marcus", alevel);


		addHandGunData.myString = "MeleeWeapon";
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
		tempAmmoReplensihData.ammoType = "MeleeWeapon";
		tempAmmoReplensihData.replenishAmount = 1000000;
		giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

		addHandGunData.myString = "PlasmaRifle";
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
		tempAmmoReplensihData.ammoType = "PlasmaRifle";
		tempAmmoReplensihData.replenishAmount = 1000;
		giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

		addHandGunData.myString = "Shotgun";
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
		tempAmmoReplensihData.ammoType = "Shotgun";
		tempAmmoReplensihData.replenishAmount = 100;
		giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

		addHandGunData.myString = "BFG";
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
		tempAmmoReplensihData.ammoType = "BFG";
		tempAmmoReplensihData.replenishAmount = 100;
		giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

		return;
	}
	CU::CJsonValue loadedLevelsStartWeaponsData = value.at(alevel);
	CU::CJsonValue weaponArray = loadedLevelsStartWeaponsData.at("Weapons");
	for (int i = 0; i < weaponArray.Size(); ++i)
	{
		std::string loadedWeaponName = weaponArray[i].at("WeaponName").GetString();
		addHandGunData.myString = loadedWeaponName.c_str();
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
		tempAmmoReplensihData.ammoType = loadedWeaponName.c_str();
		tempAmmoReplensihData.replenishAmount = 100000;
		giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);
	}
}
