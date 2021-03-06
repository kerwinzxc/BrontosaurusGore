#include "stdafx.h"
#include "PollingStation.h"
#include "AmmoPickupComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TShared/NetWorkMessage_PickupAmmo.h"
#include "../TClient/ClientMessageManager.h"
#include "../ThreadedPostmaster/GameEventMessage.h"
#include "../CommonUtilities/StringHelper.h"
#include "..\Audio\AudioInterface.h"

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
	message->SetReplenishAmount(myPickupData.replenishAmount);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(message));
	Audio::CAudioInterface::GetInstance()->PostEvent("Pickup_Ammo");

	if (myPickupData.ammoType == "BFG")
	{
		message->SetWeaponID(0);
	}
	else if (myPickupData.ammoType == "Shotgun")
	{
		message->SetWeaponID(1);
	}
	else
	{
		message->SetWeaponID(2);
	}

	if (myPickupData.ammoType == "PlasmaRifle")
	{
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"You picked up " + std::to_wstring(myPickupData.replenishAmount) + L" " + L"Gatling gun ammo!"));
		return;
	}
	if (myPickupData.ammoType == "BGF")
	{
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"You picked up " + std::to_wstring(myPickupData.replenishAmount) + L" " + L"Hand cannon ammo!"));
		return;
	}
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"You picked up " + std::to_wstring(myPickupData.replenishAmount) + L" " + CU::StringToWString(myPickupData.ammoType) + L" ammo!"));
}
