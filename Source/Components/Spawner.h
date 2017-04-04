#pragma once
#include "Component.h"
#include "EnemyTypes.h"
#include "../ThreadedPostmaster/Subscriber.h"

class CEnemy;

class CSpawnerComponent :
	public CComponent, public Postmaster::ISubscriber
{
public:
	CSpawnerComponent(const CU::GrowingArray<unsigned char>& aWaves, const eEnemyTypes aEnemyType, std::thread::id aID);
	~CSpawnerComponent();
	void Update(const float aDeltaTime);
	void SpawnEnemy();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	eMessageReturn DoEvent(const CStartWaveMessage & aStartWaveMessage) override;

private:
	CU::GrowingArray<unsigned char> myWaves;

	CEnemy* myEnemy;

	eEnemyTypes myEnemyType;

	unsigned char mySpawnerId;

	CU::TimeUnit mySpawInterval;
};

