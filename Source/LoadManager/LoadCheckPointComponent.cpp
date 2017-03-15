#include "stdafx.h"
#include "LoadCheckPointComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "CheckPointComponent.h"
#include "../Game/PlayState.h"

int LoadCheckpointComponent(KLoader::SLoadedComponentData someData)
{
	CCheckPointComponent* newCheckPointComponent = new CCheckPointComponent();
	CComponentManager::GetInstance().RegisterComponent(newCheckPointComponent);
	newCheckPointComponent->SetCheckPointPosition(someData.myData.at("SpawnPosition").GetVector3f());
	return newCheckPointComponent->GetId();

	// ifall det går för snabbt, i retail liksom
	return NULL_COMPONENT;
}
