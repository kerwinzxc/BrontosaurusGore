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
#include "CommonUtilities/CommonUtilities.h"
#include "PinkyClientDamageHandler.h"
#include "ComponentManager.h"

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
		//DL_PRINT("Didn't find your level in EnemyStats.json, please add your level to the dokument or ask Alex for help");
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

	myImpBluePrint.shootingRange = 20.0f;


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
	myRevenantBluePrint.shootingRange = 20.0f;

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
	myPinkyBluePrint.shootingRange = 20.0f;
}

CEnemy * CEnemyFactory::CreateEnemy(const eEnemyTypes & aType, const CU::Vector3f & aPosition)
{
	CGameObject* imp = myGameObjectManager.CreateGameObject();
	imp->GetLocalTransform().SetPosition(aPosition);

	CEnemy* controller = nullptr;
	CHealthComponent* health = CHealthComponentManager::GetInstance()->CreateAndRegisterComponent();
	CNetworkMessage_SpawnEnemyRepesention* message = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_SpawnEnemyRepesention>(ID_ALL);
	message->SetEnemyType(aType);

	Physics::SCharacterControllerDesc controllerDesc;
	controllerDesc.slopeLimit = 45 * (PI / 180.0f);
	controllerDesc.stepOffset = 0.3f;
	controllerDesc.skinWidth = 0.001f;
	controllerDesc.minMoveDistance = 0.f;
	controllerDesc.center.y = -0.88f;
	controllerDesc.halfHeight = 1.47f;
	controllerDesc.radius = 1.08f;

	switch (aType)
	{
	case eEnemyTypes::eImp:
	{

		controllerDesc.halfHeight = 1.96f;
		controllerDesc.radius = 1.16f;
		controllerDesc.center.y = -1.99f;

		controller = myEnemyManager.CreateComponent(&myImpBluePrint, aType);
		health->SetMaxHealth(myImpBluePrint.Health);
		health->SetHealth(myImpBluePrint.Health);
		message->SetHealth(myImpBluePrint.Health);
	}
	break;
	case eEnemyTypes::eRevenant:
	{

		controllerDesc.halfHeight = 1.47f;
		controllerDesc.radius = 1.08f;
		controllerDesc.center.y = -0.88f;


		controller = myEnemyManager.CreateComponent(&myRevenantBluePrint, aType);
		health->SetMaxHealth(myRevenantBluePrint.Health);
		health->SetHealth(myRevenantBluePrint.Health);
		message->SetHealth(myRevenantBluePrint.Health);
	}
	break;
	case eEnemyTypes::ePinky:
	{

		controllerDesc.halfHeight = 1.47f;
		controllerDesc.radius = 1.08f;
		controllerDesc.center.y = -0.88f;


		controller = myEnemyManager.CreateComponent(&myPinkyBluePrint, aType);
		health->SetMaxHealth(myPinkyBluePrint.Health);
		health->SetHealth(myPinkyBluePrint.Health);
		message->SetHealth(myPinkyBluePrint.Health);
	}
	break;
	}
	//controllerDesc.center.y += ((controllerDesc.halfHeight * 2) - (controllerDesc.radius / 2));

	controller->KillEverythingThenResetItAgain(true);
	imp->AddComponent(controller);
	imp->AddComponent(health);

	myEnemyManager.InitWeaponSystem(controller, &myWeaponSystemManager);
	message->SetPosition(aPosition);

	
	CCharacterControllerComponent* CollisionController = myColliderManager.CreateCharacterControllerComponent(controllerDesc,imp->GetId());
	imp->AddComponent(CollisionController);

	DL_PRINT("EnemyNetworkID:");
	DL_PRINT(std::to_string(controller->GetNetworkID()).c_str());

	imp->NotifyComponents(eComponentMessageType::eObjectDone, SComponentMessageData());
	Postmaster::Threaded::CPostmaster::GetInstance().BroadcastLocal(new CSendNetworkMessageMessage(message));

	return controller;
}

CEnemy* CEnemyFactory::CreateRepesention(const short aHealthValue, const eEnemyTypes aType, const CU::Vector3f& aPosition, bool aShouldReset)
{
	CGameObject* repesention = myGameObjectManager.CreateGameObject();
	repesention->GetLocalTransform().SetPosition(aPosition);

	CEnemyClientRepresentation* enemy = &CEnemyClientRepresentationManager::GetInstance().CreateAndRegister();
	enemy->SetEnemyType(aType);
	enemy->KillEverythingThenResetItAgain(true);
	repesention->AddComponent(enemy);


	Physics::SCharacterControllerDesc controllerDesc;
	controllerDesc.slopeLimit = 45 * (PI / 180.0f);
	controllerDesc.stepOffset = 0.3f;
	controllerDesc.skinWidth = 0.08f;
	controllerDesc.minMoveDistance = 0.f;
	controllerDesc.center.y = -0.88f;
	controllerDesc.radius = 1.08f;
	controllerDesc.halfHeight = 1.47f;


	CModelComponent* model = nullptr;
	switch (aType)
	{
	case eEnemyTypes::eImp:
	{
		model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_Enemy_MindControlledHuman_01.fbx");
		controllerDesc.halfHeight = 1.96f;
		controllerDesc.radius = 1.16f;
		controllerDesc.center.y = -1.99f;
	}
	break;
	case eEnemyTypes::eRevenant:
	{
		model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_Enemy_DollarDragon_01.fbx");
		controllerDesc.halfHeight = 1.47f;
		controllerDesc.radius = 1.08f;
		controllerDesc.center.y = -0.88f;
	}
	break;
	case eEnemyTypes::ePinky:
	{
		model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_Enemy_Pinky_01.fbx");
		controllerDesc.halfHeight = 1.47f;
		controllerDesc.radius = 2.08f;
		controllerDesc.center.y = -1.88f;
		CPinkyClientDamageHandler* damaageHandler = new CPinkyClientDamageHandler();
		CComponentManager::GetInstance().RegisterComponent(damaageHandler);
		repesention->AddComponent(damaageHandler);
	}
	break;
	default:
		break;
	}
	repesention->AddComponent(model);
	controllerDesc.center.x *= -1;
	controllerDesc.center.z *= -1;
	//controllerDesc.center.y -= ((controllerDesc.halfHeight * 2) - (controllerDesc.radius / 2));

	CHealthComponent* health = CHealthComponentManager::GetInstance()->CreateAndRegisterComponent();
	health->SetMaxHealth(aHealthValue);
	health->SetHealth(aHealthValue);
	DL_PRINT("Health comp ID %u", health->GetId());
	repesention->AddComponent(health);

	myEnemyManager.InitWeaponSystem(enemy, &myWeaponSystemManager);

	CCharacterControllerComponent* CollisionController = myColliderManager.CreateCharacterControllerComponent(controllerDesc, repesention->GetId());
	repesention->AddComponent(CollisionController);

	DL_PRINT("EnemyRepesentationNetworkID:");
	DL_PRINT(std::to_string(enemy->GetNetworkID()).c_str());
	repesention->GetLocalTransform().SetPosition(aPosition);
	repesention->NotifyComponents(eComponentMessageType::eObjectDone, SComponentMessageData());
	repesention->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	enemy->SetFutureMatrix(repesention->GetLocalTransform());
	if(aShouldReset == false)
	{
		enemy->DeactivateReset();
	}
	return enemy;
}
