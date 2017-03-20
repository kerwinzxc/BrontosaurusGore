#include "stdafx.h"
#include "PickupComponentManager.h"
#include "HealthPickupComponent.h"
#include "AmmoPickupComponent.h"
#include "ArmorPickupComponent.h"


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
	static int healthPickupID = 0;
	CHealthPickupComponent* healthPackComponent = new CHealthPickupComponent();
	COMPMGR.RegisterComponent(healthPackComponent);
	healthPackComponent->SetNetworkId(healthPickupID);
	healthPackComponent->SetRestoreAmount(aHealthAmount);
	myHealthPacks.emplace(healthPickupID, healthPackComponent);
	++healthPickupID;
	return healthPackComponent;
}

CAmmoPickupComponent * CPickupComponentManager::CreateAndRegisterAmmoPickupComponent(const int aAmmoAmount)
{
	static int ammoPickupID = 0;
	CAmmoPickupComponent* ammoPickupComponent = new CAmmoPickupComponent();
	COMPMGR.RegisterComponent(ammoPickupComponent);
	ammoPickupComponent->SetNetworkId(ammoPickupID);
	ammoPickupComponent->SetPickupAmount(aAmmoAmount);
	myAmmoPacks.emplace(ammoPickupID, ammoPickupComponent);
	++ammoPickupID;
	return ammoPickupComponent;
}

CArmorPickupComponent * CPickupComponentManager::CreateAndRegisterArmorPickupComponent(const armorPoint aArmorAmount)
{
	static int armorPickupID = 0;
	CArmorPickupComponent* armorPickupComponent = new CArmorPickupComponent();
	COMPMGR.RegisterComponent(armorPickupComponent);
	armorPickupComponent->SetNetworkId(armorPickupID);
	armorPickupComponent->SetReplenishAmount(aArmorAmount);
	myArmorPacks.emplace(armorPickupID, armorPickupComponent);
	++armorPickupID;
	return armorPickupComponent;
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
