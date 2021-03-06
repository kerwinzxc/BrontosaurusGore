#include "stdafx.h"
#include "ArmorPickupComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TShared/NetworkMessage_PickupArmor.h"
#include "../TClient/ClientMessageManager.h"
#include "../Game/PollingStation.h"
#include "../ThreadedPostmaster/GameEventMessage.h"
#include "..\Audio\AudioInterface.h"

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
 	message->SetReplenishAmount(myReplenishAmount);
	Audio::CAudioInterface::GetInstance()->PostEvent("Pickup_Armour");
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(message));
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"You picked up " + std::to_wstring(myReplenishAmount) + L" armor!"));
}


