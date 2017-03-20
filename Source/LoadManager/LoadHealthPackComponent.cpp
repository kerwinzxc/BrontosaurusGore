#include "stdafx.h"
#include "LoadHealthPackComponent.h"
#include "HealthPickupComponent.h"
#include "PickupComponentManager.h"
#include "ComponentManager.h"

int LoadHealthPackComponent(KLoader::SLoadedComponentData someData)
{
	static int id = 0;
	healthPoint loadedHealthValue = static_cast<healthPoint>(someData.myData.at("ReplenishAmount").GetInt());
	CHealthPickupComponent* healthPackComponent = CPickupComponentManager::GetInstance()->CreateAndRegisterHealthPickupComponent(id++, loadedHealthValue);
	return healthPackComponent->GetId();
}

