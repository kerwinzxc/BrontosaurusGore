#include "stdafx.h"
#include "LoadWeaponSystemComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "WeaponSystemComponent.h"
#include "WeaponSystemManager.h"
#include "../Game/PlayState.h"

int LoadWeaponSystemComponent(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	CWeaponSystemManager* weaponSystemManager = loadManager.GetCurrentPLaystate().GetCWeaponSystemManager();
	if(weaponSystemManager != nullptr)
	{
		CWeaponSystemComponent* newWeaponSystemComponent = weaponSystemManager->CreateAndRegisterComponent();
		return newWeaponSystemComponent->GetId();
	}
	DL_PRINT("Tried loading a weapon system component without having the manager created.");
	return NULL_COMPONENT;
}
