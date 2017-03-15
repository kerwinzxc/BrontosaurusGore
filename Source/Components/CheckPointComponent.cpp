#include "stdafx.h"
#include "CheckPointComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SetAsNewCheckPointMessage.h"
#include "../Game/PollingStation.h"

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
	case eComponentMessageType::eOnTriggerEnter :
		{
			if(myHaveBeenActivated == false)
			{
				if (CPollingStation::GetInstance()->GetPlayerObject() == aMessageData.myComponent->GetParent())
				{
					SetAsNewCheckPoint();
				}
			}
			break;
		}
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
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSetAsNewCheckPointMessage(GetParent()->GetWorldPosition() + myRespawnPosition));
}