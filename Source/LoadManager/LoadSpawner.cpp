#include "stdafx.h"
#include "LoadSpawner.h"
#include "../Components/SpawnerManager.h"
#include "../Components/Spawner.h"

int LoadSpawnerComponent(KLoader::SLoadedComponentData someData)
{

	GET_SERVERLOADMANAGER(loadManager);

	CSpawnerComponent* spawner = loadManager.GetCurrentGameServer().GetSpawnerManager()->CreateSpawnerComponent(0, 2);

	return spawner->GetId();
}
