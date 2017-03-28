#pragma once

class CSpawnerComponent;

class CSpawnerManager
{
public:
	CSpawnerManager();
	~CSpawnerManager();

	CSpawnerComponent* CreateSpawnerComponent(const unsigned char aSpawnerId, const CU::TimeUnit& aSpawnInterval);
	void Update(const float aDeltaTIme);
private:
	CU::GrowingArray<CSpawnerComponent*> mySpawners;
};

