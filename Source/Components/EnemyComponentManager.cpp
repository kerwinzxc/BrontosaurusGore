#include "stdafx.h"
#include "EnemyComponentManager.h"
#include "Enemy.h"
#include "ImpController.h"
#include "RevenantController.h"
#include "PinkyController.h"

//Temp icnludes
#include "WeaponSystemManager.h"
#include "WeaponSystemComponent.h"
#include "CharacterControllerComponent.h"
#include "ColliderComponentManager.h"
#include "../Physics/PhysicsCharacterController.h"
//

CEnemyComponentManager::CEnemyComponentManager()
{
	myEnemies.Init(10);
	myIsInited = false;
}

void CEnemyComponentManager::Update(const float aDeltaTime)
{
	for(unsigned int i = 0; i < myEnemies.Size(); ++i)
	{
		myEnemies[i]->Update(aDeltaTime);
	}
}

void CEnemyComponentManager::InitWeaponSystem(CEnemy * aEnemy, CWeaponSystemManager * aWeaponSystemManager)
{
	SComponentMessageData addWeaponData;

	aEnemy->GetParent()->AddComponent(aWeaponSystemManager->CreateAndRegisterComponent());
	Physics::SCharacterControllerDesc controllerDesc;
	controllerDesc.minMoveDistance = 0.00001f;
	controllerDesc.halfHeight = 1.0f;

	aEnemy->GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.5f, 0.0f));
	aEnemy->GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	SComponentMessageData controllerPositionData;
	controllerPositionData.myVector3f = aEnemy->GetParent()->GetLocalTransform().GetPosition();
	aEnemy->GetParent()->NotifyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
	aEnemy->GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	switch (aEnemy->GetEnemyType())
	{
	case eEnemyTypes::eImp:
	{
		addWeaponData.myString = "ImpMeleeAttack";
		aEnemy->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
		addWeaponData.myString = "ImpRangedAttack";
		aEnemy->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
		break;
	}
	case eEnemyTypes::eRevenant:
	{
		addWeaponData.myString = "RevenantRangedAttack";
		aEnemy->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
		addWeaponData.myString = "RevenantFlyingRangedAttack";
		aEnemy->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
		addWeaponData.myString = "RevenantMeleeAttack";
		aEnemy->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
		break;
	}
	case eEnemyTypes::ePinky:
	{
		addWeaponData.myString = "PinkyMeleeAttack";
		aEnemy->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
		break;
	}
	default:
		DL_ASSERT("Type not found when giving weapon from enemymanagers weaponsystem init, ask/yell at Alex/Johan");
		break;
	}
}

CEnemy* CEnemyComponentManager::CreateComponent(const SEnemyBlueprint* anEnemyBlueprint, eEnemyTypes aType)
{

	static unsigned short ID = 0;
	switch (aType)
	{
	case eEnemyTypes::eImp:
	{
		CImpController* enemy = new CImpController(ID++, aType);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;
	
	}
	case eEnemyTypes::eRevenant:
	{
		CRevenantController* enemy = new CRevenantController(ID++, aType);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;

	}
	case eEnemyTypes::ePinky:
	{
		CPinkyController* enemy = new CPinkyController(ID++, aType);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;
	}
	default:
		break;
	}

	DL_ASSERT("invalid type sent to enemycomponentManger");
	return nullptr;
}

CComponent* CEnemyComponentManager::CreateComponentAbstract(const SEnemyBlueprint* anEnemyBlueprint, eEnemyTypes aType)
{
	return CreateComponent(anEnemyBlueprint, aType);
}

void CEnemyComponentManager::DeleteComponent(CEnemy* anEnemy)
{
	myEnemies.DeleteCyclic(anEnemy);
}

CEnemyComponentManager::~CEnemyComponentManager()
{
}

void CEnemyComponentManager::Init(CWeaponSystemManager* aWeaponSystemComponentManagerPointer)
{
	for(unsigned int i = 0; i < myEnemies.Size(); i++)
	{
		InitWeaponSystem(myEnemies[i], aWeaponSystemComponentManagerPointer); 
		myEnemies[i]->Init();
	}
	myIsInited = true;
}