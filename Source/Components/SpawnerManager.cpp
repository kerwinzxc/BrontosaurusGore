#include "stdafx.h"
#include "SpawnerManager.h"
#include "Spawner.h"
#include "GameObject.h"
#include "EnemyTypes.h"

CSpawnerManager::CSpawnerManager()
{
	mySpawners.Init(100);
}


CSpawnerManager::~CSpawnerManager()
{
}

CSpawnerComponent* CSpawnerManager::CreateSpawnerComponent(const CU::GrowingArray<unsigned short>& aWaves, const eEnemyTypes& eEnemyType,  std::thread::id aID, const short aArenaID)
{
	CSpawnerComponent* spawner = new CSpawnerComponent(aWaves, eEnemyType, aID, aArenaID);
	CComponentManager::GetInstance().RegisterComponent(spawner);
	mySpawners.Add(spawner);
	return spawner;
}

void CSpawnerManager::Update(const float aDeltaTIme)
{
	for (unsigned int i = 0; i < mySpawners.Size(); ++i)
	{
		mySpawners[i]->Update(aDeltaTIme);
	}
}
