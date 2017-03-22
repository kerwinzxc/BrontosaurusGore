#include "stdafx.h"
#include "PollingStation.h"
#include "AmmoPickupComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TShared/NetWorkMessage_PickupAmmo.h"
#include "../TClient/ClientMessageManager.h"

CAmmoPickupComponent::CAmmoPickupComponent()
{
	myType = eComponentType::eAmmoPickup;
	myPickupData.ammoType = "";
	myPickupData.replenishAmount = 0;
}


CAmmoPickupComponent::~CAmmoPickupComponent()
{

}

void CAmmoPickupComponent::SetType(const std::string& aType)
{
	myPickupData.ammoType = std::string(aType);
	int br = 0;
	br++;
}

void CAmmoPickupComponent::SetPickupAmount(const unsigned short aAmount)
{
	myPickupData.replenishAmount = aAmount;
}

bool CAmmoPickupComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	return false;
}

void CAmmoPickupComponent::GiveAmmoType()
{
	SComponentMessageData data;
	SAmmoReplenishData data2;
	data2.ammoType = myPickupData.ammoType.c_str();
	data2.replenishAmount = myPickupData.replenishAmount;
	data.myAmmoReplenishData = &data2;
	CPollingStation::GetInstance()->GetPlayerObject()->NotifyComponents(eComponentMessageType::eGiveAmmo, data);
}

void CAmmoPickupComponent::DoMyEffect()
{
	GiveAmmoType();
	CNetWorkMessage_PickupAmmo* message = CClientMessageManager::GetInstance()->CreateMessage<CNetWorkMessage_PickupAmmo>(ID_ALL_BUT_ME);
	message->SetID(myNetworkId);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(message));
}
