#include "stdafx.h"
#include "LoadWeaponPickupComponent.h"
#include "WeaponPickupComponent.h"
#include "PickupComponentManager.h"

int LoadWeaponPickupkComponent(KLoader::SLoadedComponentData someData)
{
	CWeaponPickupComponent* pickup = CPickupComponentManager::GetInstance()->CreateWeaponPickupComponent(someData.myData.at("Weapon").GetString());
	return pickup->GetId();
}
