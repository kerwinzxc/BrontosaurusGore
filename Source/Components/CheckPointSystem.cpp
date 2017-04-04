#include "stdafx.h"
#include "CheckPointSystem.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SetAsNewCheckPointMessage.h"
#include "PollingStation.h"

CCheckPointSystem::CCheckPointSystem()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eAddToCheckPointRespawn);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eSetNewCheckPoint);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eResetToCheckPointMessage);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eRevivePlayer);
	myObjectsToReset.Init(50);
}


CCheckPointSystem::~CCheckPointSystem()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

eMessageReturn CCheckPointSystem::DoEvent(const CAddToCheckPointResetList& aAddToCheckPointResetList)
{
	if(myObjectsToReset.Find(aAddToCheckPointResetList.GetObjectToReset()) == myObjectsToReset.FoundNone)
	{
		myObjectsToReset.Add(aAddToCheckPointResetList.GetObjectToReset());
	}
	return eMessageReturn::eContinue;
}

eMessageReturn CCheckPointSystem::DoEvent(const CSetAsNewCheckPointMessage& aSetAsNewCheckPointMessage)
{
	myObjectsToReset.RemoveAll();
	myRespawnPlayerPosition = aSetAsNewCheckPointMessage.GetCheckPointPosition();
	return eMessageReturn::eContinue;
}

eMessageReturn CCheckPointSystem::DoEvent(const CResetToCheckPointMessage& aResetToCheckPointMessage)
{
	for(unsigned int i = 0; i < myObjectsToReset.Size(); i++)
	{
		SComponentMessageData respawnData;
		respawnData.myVector3f = myRespawnPlayerPosition;
		myObjectsToReset[i]->NotifyComponents(eComponentMessageType::eCheckPointReset, respawnData);
	}
	myObjectsToReset.RemoveAll();
	return eMessageReturn::eContinue;
}

eMessageReturn CCheckPointSystem::DoEvent(const CRevivePlayerMessage& aRevivePlayerMessage)
{
	SComponentMessageData respawnData;
	respawnData.myVector3f = myRespawnPlayerPosition;
	CPollingStation::GetInstance()->GetPlayerObject()->NotifyComponents(eComponentMessageType::eCheckPointReset, respawnData);
	return eMessageReturn::eContinue;
}