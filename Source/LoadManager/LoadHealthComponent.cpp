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
	armorPoint loadedArmorValue = static_cast<armorPoint>(someData.myData.at("MaxArmor").GetInt());
	healthComponent->SetMaxHealth(loadedHealthValue);
	healthComponent->SetHealth(loadedHealthValue);
	healthComponent->SetMaxArmor(loadedArmorValue);
	healthComponent->SetArmor(0);
	return healthComponent->GetId();
}
