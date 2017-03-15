#pragma once
#include "HealthPoint.h"

class CHealthPackComponent;
class CPickupAmmoComponent;


class CPickupComponentManager
{
public:
	static void Create();
	static void Destroy();
	static CPickupComponentManager* GetInstance();

	CHealthPackComponent* CreateAndRegisterHealthPickupComponent(const int aId, const healthPoint aHealthAmount);
	CPickupAmmoComponent* CreateAndRegisterAmmoPickupComponent(const int aId);

	CHealthPackComponent* GethHealthPackComponent(const int aId);
	CPickupAmmoComponent* GetAmmoPickupComponent(const int aId);

	void DeactivateHealthPack(const int aId);
	void DeactivateAmmoPack(const int aId);

private:
	std::map<int, CHealthPackComponent*> myHealthPacks;
	std::map<int, CPickupAmmoComponent*> myAmmoPacks;

	static CPickupComponentManager* ourInstance;
	CPickupComponentManager();
	~CPickupComponentManager();
};

