#include "stdafx.h"
#include "TriggerArenaComponent.h"
#include "PollingStation.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PlayerEnteredArena.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"

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
	case eComponentMessageType::eAddComponent:
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAddToCheckPointResetList(GetParent()));
	case eComponentMessageType::eOnTriggerEnter:
		DL_PRINT("On Trigger enter arenaaaa");
		for (unsigned int i = 0; i < CPollingStation::GetInstance()->GetNumberOfPlayers(); ++i)
		{
			DL_PRINT("On Trigger enter arenaaaa is player?");
			if (CPollingStation::GetInstance()->GetPlayers()[i] == aMessageData.myComponent->GetParent())
			{
				DL_PRINT("On Trigger enter arenaaaa is player!");
				OnPlayerEnter();
				break;
			}
		}
		break;
	case eComponentMessageType::eOnTriggerExit:
		DL_PRINT("On Trigger exit arenaaaa");
		for (unsigned int i = 0; i < CPollingStation::GetInstance()->GetNumberOfPlayers(); ++i)
		{
			if (CPollingStation::GetInstance()->GetPlayers()[i] == aMessageData.myComponent->GetParent())
			{
				OnPlayerExit();
				break;
			}
		}
		break;
	case eComponentMessageType::eCheckPointReset:
	{
		myPlayersInTrigger = 0;
	}
	break;
	default:
		break;
	}
}

void CTriggerArenaComponent::OnPlayerEnter()
{
	if (myPlayersInTrigger <= CPollingStation::GetInstance()->GetNumberOfPlayers())
	{
	myPlayersInTrigger++;
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerEnteredArena(1,myNumberOfWaves,myKeyIdToLock));
	}
}

void CTriggerArenaComponent::OnPlayerExit()
{
	if (myPlayersInTrigger <= CPollingStation::GetInstance()->GetNumberOfPlayers())
	{
		myPlayersInTrigger--;
		if (myPlayersInTrigger < 0)
		{
			myPlayersInTrigger = 0;
		}
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerEnteredArena(-1, myNumberOfWaves, myKeyIdToLock));
	}
}
