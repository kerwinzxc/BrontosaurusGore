#include "stdafx.h"
#include "LoadArmorComponent.h"
#include "PickupComponentManager.h"
#include "ArmorPickupComponent.h"
#include "ComponentManager.h"

int LoadArmorPackComponent(KLoader::SLoadedComponentData someData)
{
	healthPoint loadedArmorValue = static_cast<armorPoint>(someData.myData.at("ReplenishAmount").GetInt());
	CArmorPickupComponent* armorPackComponent = CPickupComponentManager::GetInstance()->CreateArmorPickupComponent(loadedArmorValue);
	return armorPackComponent->GetId();
}