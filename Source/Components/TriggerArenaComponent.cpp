#include "stdafx.h"
#include "TriggerArenaComponent.h"
#include "PollingStation.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PlayerEnteredArena.h"

CTriggerArenaComponent::CTriggerArenaComponent(const unsigned char aNumberOfWaves, const short aKeyId)
{
	myKeyIdToLock = aKeyId;
	myNumberOfWaves = aNumberOfWaves;
	myPlayersInTrigger = 0;
}


CTriggerArenaComponent::~CTriggerArenaComponent()
{
}

void CTriggerArenaComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnTriggerEnter:
		for (unsigned int i = 0; i < CPollingStation::GetInstance()->GetNumberOfPlayers(); ++i)
		{
			if (CPollingStation::GetInstance()->GetPlayers()[i] == aMessageData.myGameObject)
			{
				OnPlayerEnter();
				break;
			}
		}
		break;
	case eComponentMessageType::eOnTriggerExit:
		for (unsigned int i = 0; i < CPollingStation::GetInstance()->GetNumberOfPlayers(); ++i)
		{
			if (CPollingStation::GetInstance()->GetPlayers()[i] == aMessageData.myGameObject)
			{
				OnPlayerExit();
				break;
			}
		}
		break;
	default:
		break;
	}
}

void CTriggerArenaComponent::OnPlayerEnter()
{
	if (myPlayersInTrigger < CPollingStation::GetInstance()->GetNumberOfPlayers())
	{
	myPlayersInTrigger++;
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerEnteredArena(1,myNumberOfWaves,myKeyIdToLock));
	}
}

void CTriggerArenaComponent::OnPlayerExit()
{
	if (myPlayersInTrigger < CPollingStation::GetInstance()->GetNumberOfPlayers())
	{
		myPlayersInTrigger--;
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerEnteredArena(-1, myNumberOfWaves, myKeyIdToLock));
	}
}
