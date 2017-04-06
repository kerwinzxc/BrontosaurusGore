#include "stdafx.h"
#include "Enemy.h"
#include "EnemyFactory.h"
#include "EnemyComponentManager.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "ComponentMessage.h"
#include "HealthComponent.h"
#include "HealthComponentManager.h"
#include "WeaponSystemComponent.h"
#include "WeaponSystemManager.h"
#include "CharacterControllerComponent.h"
#include "ModelComponentManager.h"
#include "ColliderComponentManager.h"
#include "../Physics/PhysicsCharacterController.h"
#include "../TServer/ServerMessageManager.h"
#include "../Components/EnemyClientRepresentation.h"
#include "../Components/EnemyClientRepresentationManager.h"
#include "../CommonUtilities/JsonValue.h"


#include "TShared/NetworkMessage_SpawnEnemyRepesention.h"
#include "TServer/ServerMessageManager.h"
#include "ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "ThreadedPostmaster/Postmaster.h"


CEnemyFactory* CEnemyFactory::ourInstance = nullptr;

CEnemyFactory::CEnemyFactory(CEnemyComponentManager& aEnemyManager, CGameObjectManager& aGameObjectManger, CWeaponSystemManager& aWeaponSystemManager, CColliderComponentManager& aColliderManager)
	: myEnemyManager(aEnemyManager),
	myGameObjectManager(aGameObjectManger),
	myWeaponSystemManager(aWeaponSystemManager),
	myColliderManager(aColliderManager)
{
	//load json values for the stats and stuff of the enemies
}


CEnemyFactory::~CEnemyFactory()
{
}

void CEnemyFactory::Create(CEnemyComponentManager & aEnemyManager, CGameObjectManager & aGameObjectManger, CWeaponSystemManager & aWeaponSystemManager, CColliderComponentManager & aColliderManager)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CEnemyFactory(aEnemyManager,aGameObjectManger,aWeaponSystemManager,aColliderManager);
	}
}

void CEnemyFactory::Destroy()
{
	if (ourInstance  != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

CEnemyFactory * CEnemyFactory::GetInstance()
{
	return ourInstance;
}

void CEnemyFactory::LoadBluePrints(const std::string & alevel)
{
	CU::CJsonValue value;
	value.Parse("Json/Spawner/EnemyStats.json");
	if (value.Count(alevel) <= 0)
	{
		DL_PRINT("Didn't find your level in EnemyStats.json, please add your level to the dokument or ask Alex for help");
		return;
	}
	CU::CJsonValue impstats = value.at(alevel).at("imp");

	myImpBluePrint.Health = impstats.at("Health").GetInt();
	myImpBluePrint.speed = impstats.at("Speed").GetFloat();
	myImpBluePrint.jumpHeight = impstats.at("JumpHeight").GetFloat();
	myImpBluePrint.detectionRange = impstats.at("DetectionRange").GetFloat();
	myImpBluePrint.walkToMeleeRange = impstats.at("TryToMeleeRange").GetFloat();
	myImpBluePrint.startAttackRange = impstats.at("MeleeRange").GetFloat();
	myImpBluePrint.chargeMeleeAttackDuration = impstats.at("ChargeMeleeAttackDuration").GetFloat();
	myImpBluePrint.chargeAttackDuration = impstats.at("ChargeRangeAttackDuration").GetFloat();
	myImpBluePrint.attacksUntillRunningAway = impstats.at("NumberOfAttacksBeforeRunning").GetFloat();
	myImpBluePrint.wanderDuration = impstats.at("FleeDuration").GetFloat();
	myImpBluePrint.wanderDistance = impstats.at("FleeDistance").GetFloat();
	myImpBluePrint.wanderAngle = impstats.at("FleeAngle").GetFloat();


	CU::CJsonValue revenantStats = value.at(alevel).at("Revenant");
	myRevenantBluePrint.Health = revenantStats.at("Health").GetInt();
	myRevenantBluePrint.speed = revenantStats.at("Speed").GetFloat();
	myRevenantBluePrint.detectionRange = revenantStats.at("DetectionRange").GetFloat();
	myRevenantBluePrint.walkToMeleeRange = revenantStats.at("TryToMeleeRange").GetFloat();
	myRevenantBluePrint.startAttackRange = revenantStats.at("MeleeRange").GetFloat();
	myRevenantBluePrint.hoverTime = revenantStats.at("HoverDuration").GetFloat();
	myRevenantBluePrint.flightHeight = revenantStats.at("FlightHeight").GetFloat();
	myRevenantBluePrint.chargeAirBarrageAttackDuration = revenantStats.at("ChargeBarrageAttackDuration").GetFloat();
	myRevenantBluePrint.chargeMeleeAttackDuration = revenantStats.at("ChargeMeleeAttackDuration").GetFloat();
	myRevenantBluePrint.chargeRangedAttackAttackDuration = revenantStats.at("ChargeRangedAttackDuration").GetFloat();
	
	CU::CJsonValue pinkyStats = value.at(alevel).at("Pinky");

	myPinkyBluePrint.Health = pinkyStats.at("Health").GetInt();
	myPinkyBluePrint.speed = pinkyStats.at("Speed").GetFloat();
	myPinkyBluePrint.detectionRange = pinkyStats.at("DetectionRange").GetFloat();
	myPinkyBluePrint.walkToMeleeRange = pinkyStats.at("TryToMeleeRange").GetFloat();
	myPinkyBluePrint.startAttackRange = pinkyStats.at("MeleeRange").GetFloat();
	myPinkyBluePrint.chargeMeleeAttackDuration = pinkyStats.at("ChargeMeleeAttackDuration").GetFloat();
	myPinkyBluePrint.chargeCooldown = pinkyStats.at("ChargeCooldown").GetFloat();
	myPinkyBluePrint.chargeDistance = pinkyStats.at("ChargeDistance").GetFloat();
	myPinkyBluePrint.chargeSpeed = pinkyStats.at("ChargeSpeed").GetFloat();
	myPinkyBluePrint.windupChargeTime = pinkyStats.at("ChargingChargeDuration").GetFloat();
	myPinkyBluePrint.chargeDamage = pinkyStats.at("ChargeDamage").GetFloat();
}

CEnemy * CEnemyFactory::CreateEnemy(const eEnemyTypes & aType, const CU::Vector3f & aPosition)
{
	CGameObject* imp = myGameObjectManager.CreateGameObject();
	imp->GetLocalTransform().SetPosition(aPosition);
	aPosition.Print();

	CEnemy* controller;
	CHealthComponent* health = CHealthComponentManager::GetInstance()->CreateAndRegisterComponent();
	CNetworkMessage_SpawnEnemyRepesention* message = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_SpawnEnemyRepesention>(ID_ALL);
	message->SetEnemyType(aType);
	switch (aType)
	{
	case eEnemyTypes::eImp:
	{
		controller = myEnemyManager.CreateComponent(&myImpBluePrint, aType);
		health->SetMaxHealth(myImpBluePrint.Health);
		health->SetHealth(myImpBluePrint.Health);
		message->SetHealth(myImpBluePrint.Health);
	}
	break;
	case eEnemyTypes::eRevenant:
	{
		controller = myEnemyManager.CreateComponent(&myRevenantBluePrint, aType);
		health->SetMaxHealth(myRevenantBluePrint.Health);
		health->SetHealth(myRevenantBluePrint.Health);
		message->SetHealth(myRevenantBluePrint.Health);
	}
	break;
	case eEnemyTypes::ePinky:
	{
		controller = myEnemyManager.CreateComponent(&myPinkyBluePrint, aType);
		health->SetMaxHealth(myPinkyBluePrint.Health);
		health->SetHealth(myPinkyBluePrint.Health);
		message->SetHealth(myPinkyBluePrint.Health);
	}
	break;
	}
	controller->KillEverythingThenResetItAgain(true);
	imp->AddComponent(controller);
	imp->AddComponent(health);

	myEnemyManager.InitWeaponSystem(controller, &myWeaponSystemManager);

	Physics::SCharacterControllerDesc controllerDesc;
	controllerDesc.minMoveDistance = 0.f;
	controllerDesc.halfHeight = 1.0f;
	controllerDesc.slopeLimit = 45;
	controllerDesc.stepOffset = 0.3f;
	controllerDesc.skinWidth = 0.08f;
	controllerDesc.radius = 0.5f;
	CCharacterControllerComponent* CollisionController = myColliderManager.CreateCharacterControllerComponent(controllerDesc,imp->GetId());
	imp->AddComponent(CollisionController);

	DL_PRINT("EnemyNetworkID:");
	DL_PRINT(std::to_string(controller->GetNetworkID()).c_str());

	imp->NotifyComponents(eComponentMessageType::eObjectDone, SComponentMessageData());
	Postmaster::Threaded::CPostmaster::GetInstance().BroadcastLocal(new CSendNetworkMessageMessage(message));

	return controller;
}

CEnemy* CEnemyFactory::CreateRepesention(const short aHealthValue, const eEnemyTypes aType)
{
	CGameObject* repesention = myGameObjectManager.CreateGameObject();

	CEnemyClientRepresentation* enemy = &CEnemyClientRepresentationManager::GetInstance().CreateAndRegister();
	enemy->SetEnemyType(aType);
	enemy->KillEverythingThenResetItAgain(true);
	repesention->AddComponent(enemy);

	CModelComponent* model;
	switch (aType)
	{
	case eEnemyTypes::eImp:
	{
		model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_Enemy_MindControlledHuman_01.fbx");
	}
	break;
	case eEnemyTypes::eRevenant:
	{
		model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_Enemy_DollarDragon_01.fbx");
	}
	break;
	case eEnemyTypes::ePinky:
	{
		model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_Wagon_Wheel_01.fbx");
	}
	break;
	default:
		break;
	}
	repesention->AddComponent(model);

	CHealthComponent* health = CHealthComponentManager::GetInstance()->CreateAndRegisterComponent();
	health->SetMaxHealth(aHealthValue);
	health->SetHealth(aHealthValue);
	repesention->AddComponent(health);

	myEnemyManager.InitWeaponSystem(enemy, &myWeaponSystemManager);

	Physics::SCharacterControllerDesc controllerDesc;
	controllerDesc.minMoveDistance = 0.f;
	controllerDesc.halfHeight = 1.0f;
	controllerDesc.slopeLimit = 45;
	controllerDesc.stepOffset = 0.3f;
	controllerDesc.skinWidth = 0.08f;
	controllerDesc.radius = 0.5f;
	CCharacterControllerComponent* CollisionController = myColliderManager.CreateCharacterControllerComponent(controllerDesc, repesention->GetId());
	repesention->AddComponent(CollisionController);

	DL_PRINT("EnemyRepesentationNetworkID:");
	DL_PRINT(std::to_string(enemy->GetNetworkID()).c_str());
	repesention->NotifyComponents(eComponentMessageType::eObjectDone, SComponentMessageData());

	return enemy;
}
