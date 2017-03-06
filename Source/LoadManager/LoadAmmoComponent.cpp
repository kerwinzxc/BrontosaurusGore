#include "stdafx.h"
#include "LoadSoundComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "AmmoComponent.h"
#include "AmmoComponentManager.h"
#include "../Game/PlayState.h"

int LoadAmmoComponent(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	CAmmoComponentManager* ammoComponentManager = loadManager.GetCurrentPLaystate().GetAmmoManager();
	if(ammoComponentManager != nullptr)
	{
		CAmmoComponent* newAmmoComponent = ammoComponentManager->CreateAndRegisterComponent();
		return newAmmoComponent->GetId();
	}

	return NULL_COMPONENT;
}
