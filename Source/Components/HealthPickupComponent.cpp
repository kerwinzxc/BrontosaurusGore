#include "stdafx.h"
#include "PollingStation.h"
#include "HealthPickupComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TShared/NetworkMessage_PickupHealth.h"
#include "../TClient/ClientMessageManager.h"

CHealthPickupComponent::CHealthPickupComponent()
{
	myRestoreAmount = 0;
	myHasBeenPickedUp = false;
}


CHealthPickupComponent::~CHealthPickupComponent()
{
}

void CHealthPickupComponent::SetRestoreAmount(const healthPoint aResoreAmount)
{
	myRestoreAmount = aResoreAmount;
}

void CHealthPickupComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
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

bool CHealthPickupComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	return false;
}

void CHealthPickupComponent::RestoreHealth()
{
	SComponentMessageData data;
	data.myInt;
	GetParent()->NotifyComponents(eComponentMessageType::eHeal, data);
}

void CHealthPickupComponent::DoMyEffect()
{
	if (myHasBeenPickedUp == false)
	{
		IPickupComponent::DoMyEffect();
		RestoreHealth();
	}
}
