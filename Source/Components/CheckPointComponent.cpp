#include "stdafx.h"
#include "CheckPointComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"

CCheckPointComponent::CCheckPointComponent()
{
	//Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this);
}


CCheckPointComponent::~CCheckPointComponent()
{
	//Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
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

eMessageReturn CCheckPointComponent::DoEvent(const DrawCallsCount& aConsoleCalledupon)
{
	
	return eMessageReturn::eContinue;
}
