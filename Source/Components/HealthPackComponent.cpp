#include "stdafx.h"
#include "PollingStation.h"
#include "HealthPackComponent.h"
#include "../ThreadedPostmaster\Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TShared\NetworkMessage_PickupHealth.h"
#include "..\TClient\ClientMessageManager.h"
CHealthPackComponent::CHealthPackComponent()
{
	myRestoreAmount = 0;
	myHasBeenPickedUp = false;
}


CHealthPackComponent::~CHealthPackComponent()
{
}

void CHealthPackComponent::SetRestoreAmount(const healthPoint aResoreAmount)
{
	myRestoreAmount = aResoreAmount;
}

void CHealthPackComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnTriggerEnter:
		if (aMessageData.myComponent->GetParent() == CPollingStation::GetInstance()->GetPlayerObject())
		{
			DoMyEffect();
			CNetworkMessage_PickupHealth* message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PickupHealth>(ID_ALL_BUT_ME);
			message->SetID(myNetworkId);
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(message));
		}
	default:
		break;
	}
}

bool CHealthPackComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	return false;
}

void CHealthPackComponent::RestoreHealth()
{
	SComponentMessageData data;
	data.myInt;
	GetParent()->NotifyComponents(eComponentMessageType::eHeal, data);
}

void CHealthPackComponent::DoMyEffect()
{
	if (myHasBeenPickedUp == false)
	{
		IPickupComponent::DoMyEffect();
		RestoreHealth();
	}
}
