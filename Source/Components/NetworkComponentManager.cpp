#include "stdafx.h"
#include "NetworkComponentManager.h"
#include "NetworkComponent.h"


CNetworkComponentManager::CNetworkComponentManager()
{
}


CNetworkComponentManager::~CNetworkComponentManager()
{
}

CNetworkComponent * CNetworkComponentManager::CreateAndRegisterComponent(const unsigned aID)
{
	CNetworkComponent* networkComponent = new CNetworkComponent(aID);
	CComponentManager::GetInstance().RegisterComponent(networkComponent);
	myComponents[aID] = networkComponent;

	return networkComponent;
}
