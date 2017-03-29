#include "stdafx.h"
#include "ArmorPickupComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TShared/NetworkMessage_PickupArmor.h"
#include "../TClient/ClientMessageManager.h"
#include "../Game/PollingStation.h"

CArmorPickupComponent::CArmorPickupComponent()
{
	SetActive(true);
	myReplenishAmount = 0;
	myType = eComponentType::eArmorPickup;
}


CArmorPickupComponent::~CArmorPickupComponent()
{
}

void CArmorPickupComponent::SetReplenishAmount(const armorPoint aAmount)
{
	myReplenishAmount = aAmount;
}

void CArmorPickupComponent::DoMyEffect()
{
	SetActive(false);
	SComponentMessageData data;
	data.myInt = myReplenishAmount;
	//armor systems needs to be added, maybe healthcomponent can hold how much armor the player has
	CPollingStation::GetInstance()->GetPlayerObject()->NotifyComponents(eComponentMessageType::eAddArmor, data);
	CNetworkmessage_PickupArmor* message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkmessage_PickupArmor>(ID_ALL_BUT_ME);
	message->SetID(myNetworkId);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(message));
}


