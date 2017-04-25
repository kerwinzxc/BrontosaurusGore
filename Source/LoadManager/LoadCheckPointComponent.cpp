#include "stdafx.h"
#include "LoadCheckPointComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "CheckPointComponent.h"
#include "../Game/PlayState.h"
#include "CheckpointComponentManager.h"

int LoadCheckpointComponent(KLoader::SLoadedComponentData someData)
{
	short index = 0;
	if (someData.myData.HasKey("Index") == true)
	{
		// REMOVE THIS LATER, temp just incase LDs stuff were to get corrupted with the name change.
		index = someData.myData.at("Index").GetFloat();
	}
	CCheckPointComponent* newCheckPointComponent = CCheckpointComponentManager::GetInstance()->CreateAndRegisterComponent(index);
	newCheckPointComponent->SetCheckPointPosition(someData.myData.at("SpawnPosition").GetVector3f());
	return newCheckPointComponent->GetId();

	// ifall det går för snabbt, i retail liksom
	return NULL_COMPONENT;
}
