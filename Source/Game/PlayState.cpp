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
	, myScriptComponentManager(nullptr)
	,myExplosionComponentManager(nullptr)
	,myExplosionFactory(nullptr)
	, myIsLoaded(false)
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
	//SAFE_DELETE(myPhysics); // kanske? nope foundation förstör den
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

	Lights::SDirectionalLight dirLight;
	dirLight.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	dirLight.direction = { -1.0f, -1.0f, 1.0f, 1.0f };
	dirLight.shadowIndex = 0;
	myScene->AddDirectionalLight(dirLight);


	myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	CRenderCamera& playerCamera = myScene->GetRenderCamera(CScene::eCameraType::ePlayerOneCamera);
	playerCamera.InitPerspective(90, WINDOW_SIZE_F.x, WINDOW_SIZE_F.y, 0.1f, 500.f);

	myWeaponFactory->LoadWeapons();


	//real loading:		as opposed to fake loading
	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();

	const KLoader::eError loadError = loader.LoadFile(levelPath);
	if (loadError != KLoader::eError::NO_LOADER_ERROR)
	{
		DL_MESSAGE_BOX("Loading Failed");
	}

	CreatePlayer(playerCamera.GetCamera()); // Hard codes Player!;


	myScene->SetSkybox("default_cubemap.dds");
	myScene->SetCubemap("purpleCubemap.dds");
	
	myHUD.LoadHUD();

	myIsLoaded = true;
	
	CEnemyFactory::GetInstance()->LoadBluePrints(levelsArray.at(myLevelIndex).GetString());

	// Get time to load the level:
	loadPlaystateTimer.Update();
	float time = loadPlaystateTimer.GetDeltaTime().GetMilliseconds();
	GAMEPLAY_LOG("Game Inited in %f ms", time);
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
}

void CPlayState::Init()
{
	myCheckPointSystem = new CCheckPointSystem();
	myGameObjectManager->SendObjectsDoneMessage();
	myExplosionFactory->Init(myGameObjectManager, myModelComponentManager, myColliderComponentManager);
	CEnemyClientRepresentationManager::GetInstance().Init(myWeaponSystemManager);
	//TA BORT SENARE NÄR DET FINNS RIKTIGT GUI - johan
	myPlayerHealthText = new CTextInstance();
	myPlayerHealthText->Init();
	myPlayerHealthText->SetPosition(CU::Vector2f(0.1f, 0.825f));

	myPlayerArmorText = new CTextInstance();
	myPlayerArmorText->Init();
	myPlayerArmorText->SetPosition(CU::Vector2f(0.1f, 0.8f));
}

eStateStatus CPlayState::Update(const CU::Time& aDeltaTime)
{
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
	
	CDoorManager::GetInstance()->Update(aDeltaTime);

	myHUD.Update(aDeltaTime);

	//TA BORT SENARE NÄR DET FINNS RIKTIGT GUI - johan
	SComponentQuestionData healthData;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetHealth, healthData);
	SComponentQuestionData maxHealthData;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetMaxHealth, maxHealthData);

	myPlayerHealthText->SetText(L"Health: " +std::to_wstring(healthData.myInt) + L"/" + std::to_wstring(maxHealthData.myInt));

	SComponentQuestionData armorData;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetArmor, armorData);
	SComponentQuestionData maxArmorthData;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetMaxArmor, maxArmorthData);
	myPlayerArmorText->SetText(L"Armor: " + std::to_wstring(armorData.myInt) + L"/" + std::to_wstring(maxArmorthData.myInt));

	CTumbleweedFactory::GetInstance()->Update(aDeltaTime.GetSeconds());
	
	CAnimationComponent::UpdateAnimations(aDeltaTime);
	myScene->Update(aDeltaTime);
	if (myPhysicsScene->Simulate(aDeltaTime) == true)
	{
		myColliderComponentManager->Update();
	}

	return myStatus;
}

void CPlayState::Render()
{
	myScene->Render();

	myHUD.Render();

	myPlayerHealthText->Render();
	myPlayerArmorText->Render();
}

void CPlayState::OnEnter(const bool /*aLetThroughRender*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eChangeLevel);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
}

void CPlayState::OnExit(const bool /*aLetThroughRender*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

void CPlayState::Pause()
{
	myStateStack.PushState(new CPauseMenuState(myStateStack));
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

eMessageReturn CPlayState::DoEvent(const CLoadLevelMessage& aLoadLevelMessage)
{
	myStateStack.SwapState(new CLoadState(myStateStack, aLoadLevelMessage.myLevelIndex));
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
}

void CPlayState::SpawnOtherPlayer(unsigned aPlayerID)
{
	CGameObject* otherPlayer = myGameObjectManager->CreateGameObject();
	CGameObject* modelObject = myGameObjectManager->CreateGameObject();
	CU::Matrix44f transformation;
	transformation.RotateAroundAxis(3.14, CU::Axees::Y);
	transformation.SetPosition(0, -1.8, 0);
	modelObject->SetWorldTransformation(transformation);
	
	CModelComponent* model = myModelComponentManager->CreateComponent("Models/Meshes/M_Player_01.fbx");
	CNetworkPlayerReciverComponent* playerReciver = new CNetworkPlayerReciverComponent;
	playerReciver->SetPlayerID(aPlayerID);
	CComponentManager::GetInstance().RegisterComponent(playerReciver);

	CWeaponSystemComponent* weaponSystenComponent = myWeaponSystemManager->CreateAndRegisterComponent();
	CAmmoComponent* ammoComponent = myAmmoComponentManager->CreateAndRegisterComponent();
	otherPlayer->AddComponent(weaponSystenComponent);
	otherPlayer->AddComponent(ammoComponent);
	SComponentMessageData addHandGunData;
	SComponentMessageData giveAmmoData;

	addHandGunData.myString = "BFG";
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
	SAmmoReplenishData tempAmmoReplensihData;
	tempAmmoReplensihData.ammoType = "BFG";
	tempAmmoReplensihData.replenishAmount = 100;
	giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

	addHandGunData.myString = "Shotgun";
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
	tempAmmoReplensihData.ammoType = "Shotgun";
	tempAmmoReplensihData.replenishAmount = 100;
	giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

	addHandGunData.myString = "PlasmaRifle";
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
	tempAmmoReplensihData.ammoType = "PlasmaRifle";
	tempAmmoReplensihData.replenishAmount = 1000;
	giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

	addHandGunData.myString = "MeleeWeapon";
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
	tempAmmoReplensihData.ammoType = "MeleeWeapon";
	tempAmmoReplensihData.replenishAmount = 1000000;
	giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
	otherPlayer->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

	modelObject->AddComponent(model);
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

		CInputComponent* inputComponent = new CInputComponent();
		CComponentManager::GetInstance().RegisterComponent(inputComponent);
		playerObject->AddComponent(inputComponent);

		myMovementComponent = myMovementComponentManager->CreateAndRegisterComponent();
		playerObject->AddComponent(myMovementComponent);

		CWeaponSystemComponent* weaponSystenComponent = myWeaponSystemManager->CreateAndRegisterComponent();
		CAmmoComponent* ammoComponent = myAmmoComponentManager->CreateAndRegisterComponent();
		playerObject->AddComponent(weaponSystenComponent);
		playerObject->AddComponent(ammoComponent);
		
		GivePlayerWeapons(playerObject);

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
		CTumbleweedFactory::GetInstance()->CreateNewTumbleweed(playerObject->GetWorldPosition());

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
		addHandGunData.myString = "BFG";
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
		tempAmmoReplensihData.ammoType = "BFG";
		tempAmmoReplensihData.replenishAmount = 100;
		giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

		addHandGunData.myString = "Shotgun";
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
		tempAmmoReplensihData.ammoType = "Shotgun";
		tempAmmoReplensihData.replenishAmount = 100;
		giveAmmoData.myAmmoReplenishData = &tempAmmoReplensihData;
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

		addHandGunData.myString = "PlasmaRifle";
		aPlayerObject->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addHandGunData);
		tempAmmoReplensihData.ammoType = "PlasmaRifle";
		tempAmmoReplensihData.replenishAmount = 1000;
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