#include "stdafx.h"
#include "CheckPointSystem.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SetAsNewCheckPointMessage.h"

CCheckPointSystem::CCheckPointSystem()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eAddToCheckPointRespawn);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eSetNewCheckPoint);
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