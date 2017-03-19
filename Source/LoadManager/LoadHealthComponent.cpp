#include "stdafx.h"
#include "LoadHealthComponent.h"
#include "HealthComponent.h"
#include "ComponentManager.h"
#include "../Components/HealthComponentManager.h"

int LoadHealthComponent(KLoader::SLoadedComponentData someData)
{
	static unsigned int id = 0;

	CHealthComponent* healthComponent = CHealthComponentManager::GetInstance()->CreateAndRegisterComponent(id++);

	healthPoint loadedHealthValue = static_cast<healthPoint>(someData.myData.at("MaxHealth").GetInt());
	healthComponent->SetMaxHealth(loadedHealthValue);
	healthComponent->SetHealth(loadedHealthValue);
	return healthComponent->GetId();
}
