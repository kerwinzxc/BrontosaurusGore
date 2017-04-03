#pragma once
#include "../ThreadedPostmaster/Subscriber.h"

class CEnemy;

class CWaveManager : public Postmaster::ISubscriber
{
public:
	CWaveManager();
	~CWaveManager();

	void StartWave();

	void Update();

	eMessageReturn DoEvent(const CAddEnemyToWave & aAddEnemyToWave) override;
	eMessageReturn DoEvent(const CPlayerEnteredArena & aPlayerEnteredArena) override;

private:

	CU::GrowingArray<CEnemy*> myEnemiesInWave;

	short myKeyIDToUnlock;

	unsigned char myPlayersInsideArena;
	unsigned char myWaveCount;
	unsigned char myNumberOfPlayers;
	unsigned char myNumberOfWavesToSpawn;
};

