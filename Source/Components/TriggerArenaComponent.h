#pragma once
#include "Component.h"
class CTriggerArenaComponent :
	public CComponent
{
public:
	CTriggerArenaComponent(const unsigned char aNumberOfWaves, const short aKeyId);
	~CTriggerArenaComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
private:
	void OnPlayerEnter();
	void OnPlayerExit();
private:
	short myKeyIdToLock;
	unsigned char myNumberOfWaves;
	unsigned char myPlayersInTrigger;

};

