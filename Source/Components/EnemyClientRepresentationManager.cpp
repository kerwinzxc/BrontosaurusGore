#include "stdafx.h"
#include "EnemyClientRepresentationManager.h"
#include "WeaponSystemManager.h"
#include "WeaponSystemComponent.h"
#include "PinkyClientDamageHandler.h"

CEnemyClientRepresentationManager* CEnemyClientRepresentationManager::ourInstance = nullptr;

CEnemyClientRepresentationManager& CEnemyClientRepresentationManager::GetInstance()
{
	
	return *ourInstance;
}

CEnemyClientRepresentation& CEnemyClientRepresentationManager::CreateAndRegister()
{
	static unsigned short ID = 0;

	CEnemyClientRepresentation* rep = new CEnemyClientRepresentation(ID, eEnemyTypes::ePlaceHolder);
	myRepresentations.emplace(ID, rep);
	CComponentManager::GetInstance().RegisterComponent(rep);
	ID++;
	return *rep;
}

CEnemyClientRepresentation& CEnemyClientRepresentationManager::GetRepresentation(unsigned short aId)
{
	return *myRepresentations.at(aId);
}

void CEnemyClientRepresentationManager::Create()
{
	if(ourInstance != nullptr)
	{
		Destroy();
	}
	ourInstance = new CEnemyClientRepresentationManager;
}

void CEnemyClientRepresentationManager::Destroy()
{
	SAFE_DELETE(ourInstance);
}

CEnemyClientRepresentationManager::CEnemyClientRepresentationManager()
{
}


CEnemyClientRepresentationManager::~CEnemyClientRepresentationManager()
{
}

void CEnemyClientRepresentationManager::Update(const CU::Time& aDeltaTime)
{
	for (auto enemyRepresentation : myRepresentations)
	{
		myRepresentations.at(enemyRepresentation.first)->Update(aDeltaTime.GetSeconds());
	}
}

void CEnemyClientRepresentationManager::Init(CWeaponSystemManager* aWeaponSystemManagerPointer)
{
	SComponentMessageData addWeaponData;
	for (auto enemyRepresentation : myRepresentations)
	{
		myRepresentations.at(enemyRepresentation.first)->Init();
		myRepresentations.at(enemyRepresentation.first)->GetParent()->AddComponent(aWeaponSystemManagerPointer->CreateAndRegisterComponent());
		switch (myRepresentations.at(enemyRepresentation.first)->GetEnemyType())
		{
		case eEnemyTypes::eImp:
		{
			addWeaponData.myString = "ImpMeleeAttack";
			myRepresentations.at(enemyRepresentation.first)->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			addWeaponData.myString = "ImpRangedAttack";
			myRepresentations.at(enemyRepresentation.first)->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			break;
		}
		case eEnemyTypes::eRevenant:
		{
			addWeaponData.myString = "RevenantRangedAttack";
			myRepresentations.at(enemyRepresentation.first)->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			addWeaponData.myString = "RevenantFlyingRangedAttack";
			myRepresentations.at(enemyRepresentation.first)->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			addWeaponData.myString = "RevenantMeleeAttack";
			myRepresentations.at(enemyRepresentation.first)->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			break;
		}
		case eEnemyTypes::ePinky:
		{
			addWeaponData.myString = "PinkyMeleeAttack";
			myRepresentations.at(enemyRepresentation.first)->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			CPinkyClientDamageHandler* damaageHandler = new CPinkyClientDamageHandler();
			CComponentManager::GetInstance().RegisterComponent(damaageHandler);
			myRepresentations.at(enemyRepresentation.first)->GetParent()->AddComponent(damaageHandler);
			break;
		}
		default:
			break;
		}
	}
}