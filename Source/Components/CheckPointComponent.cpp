#include "stdafx.h"
#include "CheckPointComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SetAsNewCheckPointMessage.h"
#include "../Game/PollingStation.h"
#include "../TClient/ClientMessageManager.h"
#include "../TShared/NetworkMessage_SetCheckpointMessage.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"

CCheckPointComponent::CCheckPointComponent(const unsigned char aId)
	:myId(aId)
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
	CNetworkMessage_SetCheckpointMessage* netWorksMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_SetCheckpointMessage>(ID_ALL);
	netWorksMessage->SetId(myId);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(netWorksMessage));
}

void CCheckPointComponent::SetAsNewCheckPointWithNetwork()
{
	myHaveBeenActivated = true;
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSetAsNewCheckPointMessage(GetParent()->GetWorldPosition() + myRespawnPosition));
}