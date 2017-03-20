#pragma once
#include "HealthPoint.h"

class CHealthPickupComponent;
class CAmmoPickupComponent;
class CArmorPickupComponent;


class CPickupComponentManager
{
public:
	static void Create();
	static void Destroy();
	static CPickupComponentManager* GetInstance();

	CHealthPickupComponent* CreateAndRegisterHealthPickupComponent(const healthPoint aHealthAmount);
	CAmmoPickupComponent* CreateAndRegisterAmmoPickupComponent(const int aAmmoAmount);
	CArmorPickupComponent* CreateAndRegisterArmorPickupComponent(const armorPoint aArmorAmount);

	CHealthPickupComponent* GethHealthPackComponent(const int aId);
	CAmmoPickupComponent* GetAmmoPickupComponent(const int aId);
	CArmorPickupComponent* GetArmorPickupComponent(const int aId);

	void DeactivateHealthPack(const int aId);
	void DeactivateAmmoPack(const int aId);

private:
	std::map<int, CHealthPickupComponent*> myHealthPacks;
	std::map<int, CAmmoPickupComponent*> myAmmoPacks;
	std::map<int, CArmorPickupComponent*> myArmorPacks;

	static CPickupComponentManager* ourInstance;
	CPickupComponentManager();
	~CPickupComponentManager();
};

