#pragma once
#include "../ThreadedPostmaster/Subscriber.h"
#include <thread>

class CEnemy;

class CWaveManager : public Postmaster::ISubscriber
{
public:
	CWaveManager(const short aArenaID, const unsigned char aMaxWaves, const short aKeyID, const std::thread::id aThreadID);
	~CWaveManager();

	void StartWave();

	void Update();

	void Reset();

	void CloseArena();
	eMessageReturn DoEvent(const CAddEnemyToWave & aAddEnemyToWave) override;
	eMessageReturn DoEvent(const CPlayerEnteredArena & aPlayerEnteredArena) override;
	eMessageReturn DoEvent(const CResetToCheckPointMessage & aResetToCheckpointMessage) override;
	eMessageReturn DoEvent(const CSetAsNewCheckPointMessage& aSetAsNewCheckPointMessage) override;

private:

	CU::GrowingArray<CEnemy*> myEnemiesInWave;

	short myArenaID;
	short myKeyIDToUnlock;

	unsigned char myWaveCount;
	unsigned char myNumberOfWavesToSpawn;
};

