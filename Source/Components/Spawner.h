#pragma once
#include "Component.h"
#include "EnemyTypes.h"

class CSpawnerComponent :
	public CComponent
{
public:
	CSpawnerComponent(const CU::GrowingArray<unsigned char>& aWaves, const eEnemyTypes eEnemyType);
	~CSpawnerComponent();
	void Update(const float aDeltaTime);
	void SpawnEnemy();
private:
	CU::GrowingArray<unsigned char> myWaves;

	CEnemy* myEnemy;

	unsigned char mySpawnerId;

	CU::TimeUnit mySpawInterval;
};

