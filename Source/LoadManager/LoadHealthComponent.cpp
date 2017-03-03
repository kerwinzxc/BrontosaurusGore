#include "stdafx.h"
#include "LoadHealthComponent.h"
#include "HealthComponent.h"
#include "ComponentManager.h"

int LoadHealthComponent(KLoader::SLoadedComponentData someData)
{
	HealthComponent* healthComponent = new HealthComponent();
	COMPMGR.RegisterComponent(healthComponent);
	healthPoint loadedHealthValue = static_cast<healthPoint>(someData.myData.at("MaxHealth").GetInt());
	healthComponent->SetMaxHealth(loadedHealthValue);
	healthComponent->SetHealth(loadedHealthValue);
	return healthComponent->GetId();
}
