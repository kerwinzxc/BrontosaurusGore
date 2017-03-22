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

CHealthComponent * CHealthComponentManager::CreateAndRegisterComponent(unsigned int aID)
{
	CHealthComponent* health = new CHealthComponent(aID);

	CComponentManager::GetInstance().RegisterComponent(health);
	if (myHealthComponents.emplace(aID, health).second == false)
	{
		DL_ASSERT("Tried to insert at a already existing ID");
	}
	

	return health;
}

void CHealthComponentManager::TakeDamage(const unsigned int aID, const unsigned int aDamageTaken)
{
	SComponentMessageData data; data.myInt = aDamageTaken;
	if (myHealthComponents.count(aID) > 0)
	{
		myHealthComponents.at(aID)->Receive(eComponentMessageType::eNetworkDoDamage, data);
	}
}

CHealthComponentManager::CHealthComponentManager()
{
}


CHealthComponentManager::~CHealthComponentManager()
{
}
