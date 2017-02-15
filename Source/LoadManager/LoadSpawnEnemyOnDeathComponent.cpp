#include "stdafx.h"
#include "LoadSpawnEnemyOnDeathComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "SpawnEnemyComponent.h"

int LoadSpawnEnemiesOnDeathComponent(KLoader::SLoadedComponentData someData)
{
	CSpawnEnemyOnDeathComponent* spawn = new CSpawnEnemyOnDeathComponent(someData.myData.at("EnemiesToSpawn").GetUInt());
	CComponentManager::GetInstance().RegisterComponent(spawn);
	return spawn->GetId();
}
