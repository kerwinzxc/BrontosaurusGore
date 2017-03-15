#include "stdafx.h"
#include "LoadCheckPointComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "CheckPointComponent.h"
#include "../Game/PlayState.h"

int LoadCheckPointComponent(KLoader::SLoadedComponentData someData)
{

	CCheckPointComponent* newCheckPointComponent = new CCheckPointComponent();
	CComponentManager::GetInstance().RegisterComponent(newCheckPointComponent);
	return newCheckPointComponent->GetId();

	return NULL_COMPONENT;
}
