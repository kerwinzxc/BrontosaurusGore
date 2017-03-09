#include "stdafx.h"
#include "LoadAmmoPickupComponent.h"
#include "PickupAmmoComponent.h"
#include "ComponentManager.h"

int LoadAmmoPickupComponent(KLoader::SLoadedComponentData someData)
{
	CPickupAmmoComponent* ammoPickupComponent = new CPickupAmmoComponent();
	COMPMGR.RegisterComponent(ammoPickupComponent);
	std::string ammoType = someData.myData.at("AmmoType").GetString();
	ammoPickupComponent->SetType(ammoType);
	short replenishAmount = someData.myData.at("ReplenishAmount").GetInt();
	ammoPickupComponent->SetPickupAmount(replenishAmount);
	return ammoPickupComponent->GetId();
}

