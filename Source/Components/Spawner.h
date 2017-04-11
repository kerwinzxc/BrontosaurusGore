#pragma once
#include "Component.h"
#include "EnemyTypes.h"
#include "../ThreadedPostmaster/Subscriber.h"

class CEnemy;

class CSpawnerComponent :
	public CComponent, public Postmaster::ISubscriber
{
public:
	CSpawnerComponent(const CU::GrowingArray<unsigned short>& aWaves, const eEnemyTypes aEnemyType, std::thread::id aID, const short aArenaID);
	~CSpawnerComponent();
	void Update(const float aDeltaTime);
	void SpawnEnemy();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	eMessageReturn DoEvent(const CStartWaveMessage & aStartWaveMessage) override;

private:
	CU::GrowingArray<unsigned short> myWaves;

	CEnemy* myEnemy;

	eEnemyTypes myEnemyType;

	short myArenaID;
	unsigned char mySpawnerId;

	CU::TimeUnit mySpawInterval;
};

