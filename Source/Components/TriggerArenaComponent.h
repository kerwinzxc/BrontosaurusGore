#pragma once
#include "Component.h"
#include "../ThreadedPostmaster/Subscriber.h"
#include <thread>

class CWaveManager;

class CTriggerArenaComponent :
	public CComponent, public Postmaster::ISubscriber
{
public:
	CTriggerArenaComponent(const unsigned char aNumberOfWaves, const short aKeyId, const short aArenaID, const std::thread::id aThreadID);
	~CTriggerArenaComponent();

	void Update();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	eMessageReturn DoEvent(const CResetToCheckPointMessage & aResetToCheckpointMessage) override;
	eMessageReturn DoEvent(const CSetAsNewCheckPointMessage& aSetAsNewCheckPointMessage) override;
private:
	void OnPlayerEnter();
	void OnPlayerExit();
private:
	CWaveManager* myWaveManager;

	short myArenaID;
	short myKeyIdToLock;
	unsigned char myNumberOfWaves;
	char myPlayersInTrigger;
	bool myResetToTriggered;
	bool myHaveBeenTriggered;
	bool myHaveSubscribed;
};

