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

void CEnemyFactory::CreateImp(const SImpBlueprint * aData, const CU::Vector3f& aPosition)
{
	CGameObject* imp = myGameObjectManager.CreateGameObject();
	imp->SetWorldPosition(aPosition);

	CEnemy* controller = myEnemyManager.CreateComponent(aData,eEnemyTypes::eImp);
	imp->AddComponent(controller);
	
	CHealthComponent* health = CHealthComponentManager::GetInstance()->CreateAndRegisterComponent();
	health->SetMaxHealth(100);
	health->SetHealth(100);
	imp->AddComponent(health);
	//set health

	myEnemyManager.InitWeaponSystem(controller, &myWeaponSystemManager);

	Physics::SCharacterControllerDesc controllerDesc;
	controllerDesc.minMoveDistance = 0.f;
	controllerDesc.halfHeight = 1.0f;
	controllerDesc.slopeLimit = 45;
	controllerDesc.stepOffset = 0.3f;
	controllerDesc.skinWidth = 0.08f;
	controllerDesc.radius = 0.5f;
	CCharacterControllerComponent* CollisionController = myColliderManager.CreateCharacterControllerComponent(controllerDesc);
	imp->AddComponent(CollisionController);


	CNetworkMessage_SpawnEnemyRepesention* message = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_SpawnEnemyRepesention>(ID_ALL);
	message->SetEnemyType(eEnemyTypes::eImp);
	message->SetHealth(10);//inl�st data sen

	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(message));
}

void  CEnemyFactory::CreateRevenant(const SRevenantBlueprint * aData)
{
	CGameObject* revenant = myGameObjectManager.CreateGameObject();

	CEnemy* controller = myEnemyManager.CreateComponent(aData, eEnemyTypes::eRevenant);
	CHealthComponent* health = CHealthComponentManager::GetInstance()->CreateAndRegisterComponent();
	//set health

	myEnemyManager.InitWeaponSystem(controller, &myWeaponSystemManager);
	//CModelComponent* model = CModelComponentManager::GetInstance().CreateComponent("someJSONpath");

	Physics::SCharacterControllerDesc controllerDesc;
	controllerDesc.minMoveDistance = 0.00001f;
	controllerDesc.halfHeight = 1.0f;
	CCharacterControllerComponent* CollisionController = myColliderManager.CreateCharacterControllerComponent(controllerDesc);

	revenant->AddComponent(controller);
	revenant->AddComponent(health);
	//revenant->AddComponent(model);
	revenant->AddComponent(CollisionController);
}

void CEnemyFactory::CreatePinky(const SPinkyBlueprint * aData)
{
	CGameObject* pinky = myGameObjectManager.CreateGameObject();

	CEnemy* controller = myEnemyManager.CreateComponent(aData, eEnemyTypes::ePinky);

	CHealthComponent* health = CHealthComponentManager::GetInstance()->CreateAndRegisterComponent();

	//set health

	myEnemyManager.InitWeaponSystem(controller, &myWeaponSystemManager);
	//CModelComponent* model = CModelComponentManager::GetInstance().CreateComponent("someJSONpath");

	Physics::SCharacterControllerDesc controllerDesc;
	controllerDesc.minMoveDistance = 0.00001f;
	controllerDesc.halfHeight = 1.0f;
	CCharacterControllerComponent* CollisionController = myColliderManager.CreateCharacterControllerComponent(controllerDesc);

	pinky->AddComponent(controller);
	pinky->AddComponent(health);
	//pinky->AddComponent(model);
	pinky->AddComponent(CollisionController);
}

void CEnemyFactory::CreateRepesention(const short aHealthValue, const eEnemyTypes aType)
{
	CGameObject* repesention = myGameObjectManager.CreateGameObject();

	CEnemyClientRepresentation* enemy = &CEnemyClientRepresentationManager::GetInstance().CreateAndRegister();
	enemy->SetEnemyType(aType);
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
	health->SetMaxHealth(aHealthValue); //ta in
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
	CCharacterControllerComponent* CollisionController = myColliderManager.CreateCharacterControllerComponent(controllerDesc);
	repesention->AddComponent(CollisionController);

}