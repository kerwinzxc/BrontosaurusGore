#include "stdafx.h"
#include "PickupComponentManager.h"
#include "HealthPackComponent.h"
#include "PickupAmmoComponent.h"


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

CHealthPackComponent * CPickupComponentManager::CreateAndRegisterHealthPickupComponent(const int aId, const healthPoint aHealthAmount)
{
	CHealthPackComponent* healthPackComponent = new CHealthPackComponent();
	COMPMGR.RegisterComponent(healthPackComponent);
	healthPackComponent->SetNetworkId(aId);
	healthPackComponent->SetRestoreAmount(aHealthAmount);
	myHealthPacks.emplace(aId, healthPackComponent);
	return healthPackComponent;
}

CPickupAmmoComponent * CPickupComponentManager::CreateAndRegisterAmmoPickupComponent(const int aId)
{
	return nullptr;
}

CHealthPackComponent * CPickupComponentManager::GethHealthPackComponent(const int aId)
{
	return myHealthPacks.at(aId);
}

CPickupAmmoComponent * CPickupComponentManager::GetAmmoPickupComponent(const int aId)
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
