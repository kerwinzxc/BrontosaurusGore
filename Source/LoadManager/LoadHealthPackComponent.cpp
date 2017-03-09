#include "stdafx.h"
#include "LoadHealthPackComponent.h"
#include "HealthPackComponent.h"
#include "ComponentManager.h"

int LoadHealthPackComponent(KLoader::SLoadedComponentData someData)
{
	CHealthPackComponent* healthPackComponent = new CHealthPackComponent();
	COMPMGR.RegisterComponent(healthPackComponent);
	healthPoint loadedHealthValue = static_cast<healthPoint>(someData.myData.at("ReplenishAmount").GetInt());
	healthPackComponent->SetRestoreAmount(loadedHealthValue);
	return healthPackComponent->GetId();
}

