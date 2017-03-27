#pragma once
#include "Component.h"
class CSpawnerComponent :
	public CComponent
{
public:
	CSpawnerComponent(const unsigned char aId, const CU::TimeUnit aSpawnInterval);
	~CSpawnerComponent();
	void Update(const float aDeltaTime);
	void SpawnEnemy();
private:
	unsigned char mySpawnerId;

	CU::TimeUnit mySpawInterval;
};

