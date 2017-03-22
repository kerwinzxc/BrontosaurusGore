#include "stdafx.h"
#include "LoadDamageOnCollisionComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "DamageOnCollisionComponent.h"
#include "DamageOnCollisionComponentManager.h"
#include "../Game/PlayState.h"

int LoadDamageOnCollisionComponent(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	CDamageOnCollisionComponentManager* damageOnCollisionComponentManager = loadManager.GetCurrentPLaystate().GetDamageOnCollisionComponentManager();
	if (damageOnCollisionComponentManager != nullptr)
	{
		CDamageOnCollisionComponent* newDamageOnCollisionComponent = damageOnCollisionComponentManager->CreateAndRegisterComponent();
		healthPoint loadedHealthValue = static_cast<healthPoint>(someData.myData.at("Damage").GetFloat());
		newDamageOnCollisionComponent->SetDamage(loadedHealthValue);
		float loadedDamageCooldownValue =(someData.myData.at("Cooldown").GetFloat());
		newDamageOnCollisionComponent->SetDamageCooldown(loadedDamageCooldownValue);
		return newDamageOnCollisionComponent->GetId();
	}

	return NULL_COMPONENT;
}
