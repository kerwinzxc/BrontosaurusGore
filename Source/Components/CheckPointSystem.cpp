#include "stdafx.h"
#include "CheckPointSystem.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SetAsNewCheckPointMessage.h"

CCheckPointSystem::CCheckPointSystem()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eAddToCheckPointRespawn);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eSetNewCheckPoint);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eResetToCheckPointMessage);
	myObjectsToReset.Init(50);
}


CCheckPointSystem::~CCheckPointSystem()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

eMessageReturn CCheckPointSystem::DoEvent(const CAddToCheckPointResetList& aAddToCheckPointResetList)
{
	myObjectsToReset.Add(aAddToCheckPointResetList.GetObjectToReset());
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