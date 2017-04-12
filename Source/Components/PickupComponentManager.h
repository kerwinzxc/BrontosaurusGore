#pragma once
#include "HealthPoint.h"

class CHealthPickupComponent;
class CAmmoPickupComponent;
class CArmorPickupComponent;
class CKeyPickupComponent;
class CWeaponPickupComponent;

class CPickupComponentManager
{
public:
	static void Create();
	static void Destroy();
	static CPickupComponentManager* GetInstance();

	CHealthPickupComponent* CreateHealthPickupComponent(const healthPoint aHealthAmount);
	CAmmoPickupComponent* CreateAmmoPickupComponent(const int aAmmoAmount, const std::string& aAmmoType);
	CArmorPickupComponent* CreateArmorPickupComponent(const armorPoint aArmorAmount);
	CKeyPickupComponent* CreateKeyPickupComponent(const short aKeyId);
	CWeaponPickupComponent* CreateWeaponPickupComponent(const std::string& aWeaponName);

	CHealthPickupComponent* GethHealthPackComponent(const int aId);
	CAmmoPickupComponent* GetAmmoPickupComponent(const int aId);
	CArmorPickupComponent* GetArmorPickupComponent(const int aId);
	CKeyPickupComponent* GetKeyPickupComponent(const int aId);
	CWeaponPickupComponent* GetWeaponPickupComponent(const int aId);

	void DeactivateHealthPack(const int aId);
	void DeactivateAmmoPack(const int aId);
	void DeactivateArmorPack(const int aId);
	void DeactivateKeyPickup(const int aId);
	void DeactivateWeaponPickup(const int aId);
	void Update(const float aDeltaTime);
private:
	std::map<int, CHealthPickupComponent*> myHealthPacks;
	std::map<int, CAmmoPickupComponent*> myAmmoPacks;
	std::map<int, CArmorPickupComponent*> myArmorPacks;
	std::map<int, CKeyPickupComponent*> myKeyPickups;
	std::map<int, CWeaponPickupComponent*> myWeaponPickups;

	static CPickupComponentManager* ourInstance;
	CPickupComponentManager();
	~CPickupComponentManager();
};

