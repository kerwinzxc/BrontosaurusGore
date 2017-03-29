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
	mySpawners.DeleteAll();
}

CSpawnerComponent* CSpawnerManager::CreateSpawnerComponent(const CU::GrowingArray<unsigned char>& aWaves, const eEnemyTypes& eEnemyType)
{
	CSpawnerComponent* spawner = new CSpawnerComponent(aWaves, eEnemyType);
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
