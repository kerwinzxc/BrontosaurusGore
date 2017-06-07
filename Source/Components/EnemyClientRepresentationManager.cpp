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

unsigned short CEnemyClientRepresentationManager::ourID = 0;

CEnemyClientRepresentation& CEnemyClientRepresentationManager::CreateAndRegister()
{
	CEnemyClientRepresentation* rep = new CEnemyClientRepresentation(ourID, eEnemyTypes::ePlaceHolder);
	myRepresentations.emplace(ourID, rep);
	CComponentManager::GetInstance().RegisterComponent(rep);
	ourID++;
	return *rep;
}

static CEnemyClientRepresentation ourNullClientRepresentation(-1, eEnemyTypes::ePlaceHolder);
static CGameObject ourNullGameObject(&ourNullClientRepresentation);

CEnemyClientRepresentation& CEnemyClientRepresentationManager::GetRepresentation(unsigned short aId)
{
	if (myRepresentations.find(aId) != myRepresentations.end())
	{
		return *myRepresentations.at(aId);
	}

	DL_PRINT("nullcomponent %d", aId);
	return ourNullClientRepresentation;
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
	for (auto& enemyRepresentation : myRepresentations)
	{
		enemyRepresentation.second->Update(aDeltaTime.GetSeconds());
	}
}

void CEnemyClientRepresentationManager::Init(CWeaponSystemManager* aWeaponSystemManagerPointer)
{
	SComponentMessageData addWeaponData;
	for (auto& enemyRepresentation : myRepresentations)
	{
		enemyRepresentation.second->Init();
		enemyRepresentation.second->GetParent()->AddComponent(aWeaponSystemManagerPointer->CreateAndRegisterComponent());
		switch (enemyRepresentation.second->GetEnemyType())
		{
		case eEnemyTypes::eImp:
		{
			addWeaponData.myString = "ImpMeleeAttack";
			enemyRepresentation.second->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			addWeaponData.myString = "ImpRangedAttack";
			enemyRepresentation.second->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			break;
		}
		case eEnemyTypes::eRevenant:
		{
			addWeaponData.myString = "RevenantRangedAttack";
			enemyRepresentation.second->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			addWeaponData.myString = "RevenantFlyingRangedAttack";
			enemyRepresentation.second->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			addWeaponData.myString = "RevenantMeleeAttack";
			enemyRepresentation.second->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			break;
		}
		case eEnemyTypes::ePinky:
		{
			addWeaponData.myString = "PinkyMeleeAttack";
			enemyRepresentation.second->GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddWeaponWithoutChangingToIt, addWeaponData);
			CPinkyClientDamageHandler* damaageHandler = new CPinkyClientDamageHandler();
			CComponentManager::GetInstance().RegisterComponent(damaageHandler);
			enemyRepresentation.second->GetParent()->AddComponent(damaageHandler);
			break;
		}
		default:
			break;
		}
	}
}

void CEnemyClientRepresentationManager::CleanUpRepresentations()
{
	ourID = 0;
	myRepresentations.clear();
}

bool CEnemyClientRepresentationManager::CheckIfRepresentationExists(unsigned short aId)
{
	if(myRepresentations.find(aId) == myRepresentations.end())
	{
		return false;
	}
	return true;
}