#include "stdafx.h"
#include "SpawnerManager.h"
#include "Spawner.h"
#include "GameObject.h"

CSpawnerManager::CSpawnerManager()
{
	mySpawners.Init(100);
}


CSpawnerManager::~CSpawnerManager()
{
	mySpawners.DeleteAll();
}

CSpawnerComponent* CSpawnerManager::CreateSpawnerComponent(const unsigned char aSpawnerId, const CU::TimeUnit & aSpawnInterval)
{
	CSpawnerComponent* spawner = new CSpawnerComponent(aSpawnerId, aSpawnInterval);
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
