#pragma once

class CSpawnerComponent;

class CSpawnerManager
{
public:
	CSpawnerManager();
	~CSpawnerManager();

	CSpawnerComponent* CreateSpawnerComponent(const CU::GrowingArray<unsigned char>& aWaves, const eEnemyTypes eEnemyType);
	void Update(const float aDeltaTIme);
private:
	CU::GrowingArray<CSpawnerComponent*> mySpawners;
};

