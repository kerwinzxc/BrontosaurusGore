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

	CHealthPickupComponent* CreateHealthPickupComponent(const healthPoint aHealthAmount);
	CAmmoPickupComponent* CreateAmmoPickupComponent(const int aAmmoAmount, const std::string& aAmmoType);
	CArmorPickupComponent* CreateArmorPickupComponent(const armorPoint aArmorAmount);

	CHealthPickupComponent* GethHealthPackComponent(const int aId);
	CAmmoPickupComponent* GetAmmoPickupComponent(const int aId);
	CArmorPickupComponent* GetArmorPickupComponent(const int aId);

	void DeactivateHealthPack(const int aId);
	void DeactivateAmmoPack(const int aId);
	void DeactivateArmorPack(const int aId);
private:
	std::map<int, CHealthPickupComponent*> myHealthPacks;
	std::map<int, CAmmoPickupComponent*> myAmmoPacks;
	std::map<int, CArmorPickupComponent*> myArmorPacks;

	static CPickupComponentManager* ourInstance;
	CPickupComponentManager();
	~CPickupComponentManager();
};

