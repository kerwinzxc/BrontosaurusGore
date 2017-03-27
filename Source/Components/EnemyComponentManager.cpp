#include "stdafx.h"
#include "EnemyComponentManager.h"
#include "Enemy.h"
#include "ImpController.h"
#include "RevenantController.h"
#include "PinkyController.h"

//Temp icnludes
#include "WeaponSystemManager.h"
#include "WeaponSystemComponent.h"
#include "CharcterControllerComponent.h"
#include "ColliderComponentManager.h"
#include "../Physics/PhysicsCharacterController.h"
//

CEnemyComponentManager::CEnemyComponentManager()
{
	myEnemies.Init(10);
}

void CEnemyComponentManager::Update(const float aDeltaTime)
{
	for(unsigned int i = 0; i < myEnemies.Size(); ++i)
	{
		myEnemies[i]->Update(aDeltaTime);
	}
}

CEnemy* CEnemyComponentManager::CreateComponent(const SEnemyBlueprint* anEnemyBlueprint, unsigned int anId, eEnemyTypes aType)
{
	switch (aType)
	{
	case eEnemyTypes::eDefault:
	{
		CEnemy* enemy = new CEnemy(anId, aType);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;

	}
	case eEnemyTypes::eImp:
	{
		CImpController* enemy = new CImpController(anId, aType);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;
	
	}
	case eEnemyTypes::eRevenant:
	{
		CRevenantController* enemy = new CRevenantController(anId, aType);
		enemy->SetEnemyData(anEnemyBlueprint);

		myEnemies.Add(enemy);

		CComponentManager::GetInstance().RegisterComponent(enemy);

		return enemy;
		break;

	}
	case eEnemyTypes::ePinky:
	{
		CPinkyController* enemy = new CPinkyController(anId, aType);
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

CComponent* CEnemyComponentManager::CreateComponentAbstract(const SEnemyBlueprint* anEnemyBlueprint, unsigned int anId, eEnemyTypes aType)
{
	return CreateComponent(anEnemyBlueprint,anId, aType);
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
	SComponentMessageData addWeaponData;

	for(unsigned int i = 0; i < myEnemies.Size(); i++)
	{
		myEnemies[i]->GetParent()->AddComponent(aWeaponSystemComponentManagerPointer->CreateAndRegisterComponent());
		Physics::SCharacterControllerDesc controllerDesc;
		controllerDesc.minMoveDistance = 0.00001f;
		controllerDesc.halfHeight = 1.0f;

		myEnemies[i]->GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.5f, 0.0f));
		myEnemies[i]->GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		SComponentMessageData controllerPositionData;
		controllerPositionData.myVector3f = myEnemies[i]->GetParent()->GetLocalTransform().GetPosition();
		myEnemies[i]->GetParent()->NotifyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
		myEnemies[i]->GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		switch (myEnemies[i]->GetEnemyType())
		{
		case eEnemyTypes::eImp:
		{
			addWeaponData.myString = "ImpMeleeAttack";
			myEnemies[i]->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
			addWeaponData.myString = "ImpRangedAttack";
			myEnemies[i]->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
			break;
		}
		case eEnemyTypes::eRevenant:
		{
			addWeaponData.myString = "RevenantRangedAttack";
			myEnemies[i]->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
			addWeaponData.myString = "RevenantFlyingRangedAttack";
			myEnemies[i]->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
			addWeaponData.myString = "RevenantMeleeAttack";
			myEnemies[i]->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
			break;
		}
		case eEnemyTypes::ePinky:
		{
			addWeaponData.myString = "PinkyMeleeAttack";
			myEnemies[i]->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeapon, addWeaponData);
			break;
		}
		default:
			break;
		}
	}
}