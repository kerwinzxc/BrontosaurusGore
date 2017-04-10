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
	myHaveBeenTriggered = false;
	myResetToTriggered = false;

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eSetNewCheckPoint);
	
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
		//DL_PRINT("On Trigger enter arenaaaa");
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
		myHaveBeenTriggered = myResetToTriggered;
	}
	break;
	default:
		break;
	}
}

void CTriggerArenaComponent::OnPlayerEnter()
{
	if (myPlayersInTrigger <= CPollingStation::GetInstance()->GetNumberOfPlayers() && myHaveBeenTriggered == false)
	{
		DL_PRINT("EnteredTriggerArena");

		myPlayersInTrigger++;
		DL_PRINT("NumberOfPlayersINside");
		DL_PRINT(std::to_string(myPlayersInTrigger).c_str());
		if (myPlayersInTrigger == CPollingStation::GetInstance()->GetNumberOfPlayers())
		{
			myHaveBeenTriggered = true;
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerEnteredArena(1, myNumberOfWaves, myKeyIdToLock));
			return;
		}
	
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerEnteredArena(1,0,myKeyIdToLock));
	}

	//Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerEnteredArena(1, myNumberOfWaves, myKeyIdToLock));
}

void CTriggerArenaComponent::OnPlayerExit()
{
	if (myPlayersInTrigger <= CPollingStation::GetInstance()->GetNumberOfPlayers() && myHaveBeenTriggered == false)
	{
		if (myPlayersInTrigger == CPollingStation::GetInstance()->GetNumberOfPlayers())
		{
			//Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerEnteredArena(-1, -myNumberOfWaves, myKeyIdToLock));
			myPlayersInTrigger--;
			if (myPlayersInTrigger < 0)
			{
				myPlayersInTrigger = 0;
			}
			DL_PRINT("NumberOfPlayersINside:");
			DL_PRINT(std::to_string(myPlayersInTrigger).c_str());
			return;
		}
		myPlayersInTrigger--;
		if (myPlayersInTrigger < 0)
		{
			myPlayersInTrigger = 0;
		}
		DL_PRINT("NumberOfPlayersINside:");
		DL_PRINT(std::to_string(myPlayersInTrigger).c_str());
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerEnteredArena(-1, 0, myKeyIdToLock));
	}
}

eMessageReturn CTriggerArenaComponent::DoEvent(const CResetToCheckPointMessage & aResetToCheckpointMessage)
{
	myResetToTriggered = myHaveBeenTriggered;
	return eMessageReturn::eContinue;
}