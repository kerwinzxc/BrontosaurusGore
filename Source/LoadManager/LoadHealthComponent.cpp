#include "stdafx.h"
#include "LoadHealthComponent.h"
#include "HealthComponent.h"
#include "ComponentManager.h"
#include "../Components/HealthComponentManager.h"

int LoadHealthComponent(KLoader::SLoadedComponentData someData)
{
	CHealthComponent* healthComponent = CHealthComponentManager::GetInstance()->CreateAndRegisterComponent();
	healthPoint loadedHealthValue = static_cast<healthPoint>(someData.myData.at("MaxHealth").GetInt());
	healthComponent->SetMaxHealth(loadedHealthValue);
	healthComponent->SetHealth(loadedHealthValue);
	if (someData.myData.HasKey("MaxArmor") == true)
	{
		armorPoint loadedArmorValue = static_cast<armorPoint>(someData.myData.at("MaxArmor").GetInt());
		healthComponent->SetMaxArmor(loadedArmorValue);
		healthComponent->SetArmor(0);
	}
	return healthComponent->GetId();
}
