#include "stdafx.h"
#include "LoadAmmoPickupComponent.h"
#include "PickupComponentManager.h"
#include "AmmoPickupComponent.h"
#include "ComponentManager.h"

int LoadAmmoPickupComponent(KLoader::SLoadedComponentData someData)
{
	std::string ammoType = someData.myData.at("AmmoType").GetString();
	short replenishAmount = someData.myData.at("ReplenishAmount").GetInt();
	CAmmoPickupComponent* ammoPickupComponent = CPickupComponentManager::GetInstance()->CreateAmmoPickupComponent(replenishAmount,ammoType);
	return ammoPickupComponent->GetId();
}

