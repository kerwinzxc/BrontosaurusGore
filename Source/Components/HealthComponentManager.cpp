#include "stdafx.h"
#include "HealthComponentManager.h"
#include "../Components/HealthComponent.h"


CHealthComponentManager* CHealthComponentManager::ourInstance = nullptr;

void CHealthComponentManager::Create()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CHealthComponentManager;
	}
}

void CHealthComponentManager::Destroy()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

CHealthComponentManager * CHealthComponentManager::GetInstance()
{
	return ourInstance;
}

CHealthComponent * CHealthComponentManager::CreateAndRegisterComponent()
{
	static unsigned short ID = 0;

	CHealthComponent* health = new CHealthComponent(ID);

	CComponentManager::GetInstance().RegisterComponent(health);
	if (myHealthComponents.emplace(ID, health).second == false)
	{
		DL_ASSERT("Tried to insert at a already existing ID");
	}
	ID++;

	return health;
}

void CHealthComponentManager::TakeDamage(const unsigned int aID, const unsigned int aDamageTaken)
{
	SComponentMessageData data; data.myInt = aDamageTaken;
	if (myHealthComponents.count(aID) > 0)
	{
		myHealthComponents.at(aID)->GetParent()->NotifyOnlyComponents(eComponentMessageType::eNetworkDoDamage, data);
	}
}

CHealthComponentManager::CHealthComponentManager()
{
}


CHealthComponentManager::~CHealthComponentManager()
{
}
