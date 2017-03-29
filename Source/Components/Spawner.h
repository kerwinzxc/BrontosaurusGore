#pragma once
#include "Component.h"
#include "EnemyTypes.h"

class CEnemy;

class CSpawnerComponent :
	public CComponent
{
public:
	CSpawnerComponent(const CU::GrowingArray<unsigned char>& aWaves, const eEnemyTypes aEnemyType);
	~CSpawnerComponent();
	void Update(const float aDeltaTime);
	void SpawnEnemy();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
private:
	CU::GrowingArray<unsigned char> myWaves;

	CEnemy* myEnemy;

	eEnemyTypes myEnemyType;

	unsigned char mySpawnerId;

	CU::TimeUnit mySpawInterval;
};

