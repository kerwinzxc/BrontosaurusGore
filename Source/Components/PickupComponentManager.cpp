#include "stdafx.h"
#include "PickupComponentManager.h"
#include "HealthPickupComponent.h"
#include "AmmoPickupComponent.h"
#include "ArmorPickupComponent.h"
#include "KeyPickupComponent.h"
#include "WeaponPickupComponent.h"

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

CHealthPickupComponent * CPickupComponentManager::CreateHealthPickupComponent(const healthPoint aHealthAmount)
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

CAmmoPickupComponent * CPickupComponentManager::CreateAmmoPickupComponent(const int aAmmoAmount, const std::string& aAmmoType)
{
	static int ammoPickupID = 0;
	CAmmoPickupComponent* ammoPickupComponent = new CAmmoPickupComponent();
	COMPMGR.RegisterComponent(ammoPickupComponent);
	ammoPickupComponent->SetNetworkId(ammoPickupID);
	ammoPickupComponent->SetPickupAmount(aAmmoAmount);
	ammoPickupComponent->SetType(aAmmoType);
	myAmmoPacks.emplace(ammoPickupID, ammoPickupComponent);
	++ammoPickupID;
	return ammoPickupComponent;
}

CArmorPickupComponent * CPickupComponentManager::CreateArmorPickupComponent(const armorPoint aArmorAmount)
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

CKeyPickupComponent * CPickupComponentManager::CreateKeyPickupComponent(const short aKeyId)
{
	static int keyPickupId = 0;
	CKeyPickupComponent* keyPickup = new CKeyPickupComponent();
	keyPickup->SetLockId(aKeyId);
	keyPickup->SetNetworkId(keyPickupId);
	COMPMGR.RegisterComponent(keyPickup);
	myKeyPickups.emplace(keyPickupId++, keyPickup);
	return keyPickup;
}

CWeaponPickupComponent* CPickupComponentManager::CreateWeaponPickupComponent(const std::string& aWeaponName)
{
	static int weaponPickupId = 0;
	CWeaponPickupComponent* weapon = new CWeaponPickupComponent();
	weapon->SetWeaponPickup(aWeaponName);
	weapon->SetNetworkId(weaponPickupId);
	COMPMGR.RegisterComponent(weapon);
	myWeaponPickups.emplace(weaponPickupId++, weapon);
	return weapon;
}

CHealthPickupComponent * CPickupComponentManager::GethHealthPackComponent(const int aId)
{
	return myHealthPacks.at(aId);
}

CAmmoPickupComponent * CPickupComponentManager::GetAmmoPickupComponent(const int aId)
{
	return myAmmoPacks.at(aId);
}

CArmorPickupComponent * CPickupComponentManager::GetArmorPickupComponent(const int aId)
{
	return myArmorPacks.at(aId);
}

CKeyPickupComponent * CPickupComponentManager::GetKeyPickupComponent(const int aId)
{
	return myKeyPickups.at(aId);
}
CWeaponPickupComponent* CPickupComponentManager::GetWeaponPickupComponent(const int aId)
{
	return myWeaponPickups.at(aId);
}

void CPickupComponentManager::DeactivateHealthPack(const int aId)
{
	if (myHealthPacks.find(aId) != myHealthPacks.end())
	{
		myHealthPacks.at(aId)->SetActive(false);
	}
}

void CPickupComponentManager::DeactivateAmmoPack(const int aId)
{
	if (myAmmoPacks.find(aId) != myAmmoPacks.end())
	{
		myAmmoPacks.at(aId)->SetActive(false);
	}
}

void CPickupComponentManager::DeactivateArmorPack(const int aId)
{
	if(myArmorPacks.find(aId) != myArmorPacks.end())
	{
		myArmorPacks.at(aId)->SetActive(false);
	}
}

void CPickupComponentManager::DeactivateKeyPickup(const int aId)
{
	if (myKeyPickups.find(aId) != myKeyPickups.end())
	{
		myKeyPickups.at(aId)->SetActive(false);
	}
}
void CPickupComponentManager::DeactivateWeaponPickup(const int aId)
{
	if (myWeaponPickups.find(aId) != myWeaponPickups.end())
	{
		myWeaponPickups.at(aId)->SetActive(false);
	}
}

void CPickupComponentManager::Update(const float aDeltaTime)
{
	for (std::pair<const int, IPickupComponent*> comp : myAmmoPacks)
	{
		comp.second->Update(aDeltaTime);
	}
	for (std::pair<const int, IPickupComponent*> comp : myArmorPacks)
	{
		comp.second->Update(aDeltaTime);
	}
	for (std::pair<const int, IPickupComponent*> comp : myHealthPacks)
	{
		comp.second->Update(aDeltaTime);
	}
	for (std::pair<const int, IPickupComponent*> comp : myKeyPickups)
	{
		comp.second->Update(aDeltaTime);
	}
	for (std::pair<const int, IPickupComponent*> comp : myWeaponPickups)
	{
		comp.second->Update(aDeltaTime);
	}
}



CPickupComponentManager::CPickupComponentManager()
{
}

CPickupComponentManager::~CPickupComponentManager()
{
}
