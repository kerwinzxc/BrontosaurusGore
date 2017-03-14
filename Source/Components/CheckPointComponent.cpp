#include "stdafx.h"
#include "CheckPointComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SetAsNewCheckPointMessage.h"

CCheckPointComponent::CCheckPointComponent()
{
	myHaveBeenActivated = false;
}


CCheckPointComponent::~CCheckPointComponent()
{
}

void CCheckPointComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	aMessageData;
	switch (aMessageType)
	{
	default:
		break;
	}
}
void CCheckPointComponent::Destroy()
{

}
void CCheckPointComponent::Update(float aDeltaTime)
{
	aDeltaTime;
}

void CCheckPointComponent::SetAsNewCheckPoint()
{
	myHaveBeenActivated = true;
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSetAsNewCheckPointMessage(myRespawnPosition));
}