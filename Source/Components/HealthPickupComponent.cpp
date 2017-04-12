#include "stdafx.h"
#include "PollingStation.h"
#include "HealthPickupComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../TShared/NetworkMessage_PickupHealth.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TClient/ClientMessageManager.h"
#include "../ThreadedPostmaster/GameEventMessage.h"
#include "..\Audio\AudioInterface.h"

CHealthPickupComponent::CHealthPickupComponent()
{
	myRestoreAmount = 0;
	myHasBeenPickedUp = false;
	myType = eComponentType::eHealthPickupComponent;
}


CHealthPickupComponent::~CHealthPickupComponent()
{
}

void CHealthPickupComponent::SetRestoreAmount(const healthPoint aResoreAmount)
{
	myRestoreAmount = aResoreAmount;
}

bool CHealthPickupComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	return false;
}

void CHealthPickupComponent::RestoreHealth()
{
	SComponentMessageData data;
	data.myInt = myRestoreAmount;
	CPollingStation::GetInstance()->GetPlayerObject()->NotifyComponents(eComponentMessageType::eHeal, data);
}

void CHealthPickupComponent::DoMyEffect()
{
	RestoreHealth();
	CNetworkMessage_PickupHealth* message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PickupHealth>(ID_ALL_BUT_ME);
	message->SetID(myNetworkId);
	message->SetReplenishAmount(myRestoreAmount);
	Audio::CAudioInterface::GetInstance()->PostEvent("Pickup_Health");
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(message));
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"You picked up " + std::to_wstring(myRestoreAmount) + L" health!"));
}
