#include "stdafx.h"
#include "LoadHealthPackComponent.h"
#include "HealthPickupComponent.h"
#include "PickupComponentManager.h"
#include "ComponentManager.h"

int LoadHealthPackComponent(KLoader::SLoadedComponentData someData)
{
	healthPoint loadedHealthValue = static_cast<healthPoint>(someData.myData.at("ReplenishAmount").GetInt());
	CHealthPickupComponent* healthPackComponent = CPickupComponentManager::GetInstance()->CreateAndRegisterHealthPickupComponent(loadedHealthValue);
	return healthPackComponent->GetId();
}

