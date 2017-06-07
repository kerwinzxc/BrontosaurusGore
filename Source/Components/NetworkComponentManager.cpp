#include "stdafx.h"
#include "NetworkComponentManager.h"
#include "NetworkComponent.h"

CNetworkComponentManager * CNetworkComponentManager::ourInstance = nullptr;

CNetworkComponentManager::CNetworkComponentManager()
{
}


CNetworkComponentManager::~CNetworkComponentManager()
{
}

void CNetworkComponentManager::Create()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CNetworkComponentManager;
	}
	else
	{
		DL_ASSERT("network component manager not created");
	}
}

void CNetworkComponentManager::Destroy()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
	else
	{
		DL_ASSERT("newtork component manager not created");
	}
}

CNetworkComponentManager * CNetworkComponentManager::GetInstance()
{
	return ourInstance;
}

CNetworkComponent * CNetworkComponentManager::CreateAndRegisterComponent(const unsigned aID)
{
	CNetworkComponent* networkComponent = new CNetworkComponent(aID);
	CComponentManager::GetInstance().RegisterComponent(networkComponent);
	myComponents[aID] = networkComponent;

	return networkComponent;
}

CNetworkComponent * CNetworkComponentManager::GetComponent(const unsigned aID)
{
	return myComponents[aID];
}
