#include "stdafx.h"
#include "PickupComponentManager.h"
#include "HealthPickupComponent.h"
#include "AmmoPickupComponent.h"


CPickupComponentManager* CPickupComponentManager::ourInstance = nullptr;

void CPickupComponentManager::Create()
{
	if(ourInstance == nullptr)
	{
		ourInstance = new CPickupComponentManager();
	}
}

void CPickupComponentManager::Destroy()
{
	if(ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

CPickupComponentManager * CPickupComponentManager::GetInstance()
{
	return ourInstance;
}

CHealthPickupComponent * CPickupComponentManager::CreateAndRegisterHealthPickupComponent(const healthPoint aHealthAmount)
{
	static int id = 0;
	CHealthPickupComponent* healthPackComponent = new CHealthPickupComponent();
	COMPMGR.RegisterComponent(healthPackComponent);
	healthPackComponent->SetNetworkId(id);
	healthPackComponent->SetRestoreAmount(aHealthAmount);
	myHealthPacks.emplace(id, healthPackComponent);
	++id;
	return healthPackComponent;
}

CAmmoPickupComponent * CPickupComponentManager::CreateAndRegisterAmmoPickupComponent(const int aId)
{
	return nullptr;
}

CHealthPickupComponent * CPickupComponentManager::GethHealthPackComponent(const int aId)
{
	return myHealthPacks.at(aId);
}

CAmmoPickupComponent * CPickupComponentManager::GetAmmoPickupComponent(const int aId)
{
	return myAmmoPacks.at(aId);
}

void CPickupComponentManager::DeactivateHealthPack(const int aId)
{
	myHealthPacks.at(aId)->SetActive(false);
}

void CPickupComponentManager::DeactivateAmmoPack(const int aId)
{
	myAmmoPacks.at(aId)->SetActive(false);
}

CPickupComponentManager::CPickupComponentManager()
{
}


CPickupComponentManager::~CPickupComponentManager()
{
}
