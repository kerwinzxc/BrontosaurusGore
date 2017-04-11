#include "stdafx.h"
#include "WeaponPickupComponent.h"
#include "PollingStation.h"
#include "../TShared/NetworkMessage_PickupWeapon.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../TClient/ClientMessageManager.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../ThreadedPostmaster/GameEventMessage.h"
#include "AmmoReplenishData.h"
#include "WeaponPickupData.h"

CWeaponPickupComponent::CWeaponPickupComponent()
{
	myNetworkId = -1;
	myType = eComponentType::eWeaponPickupComponent;
	myWeaponPickup = "";
}


CWeaponPickupComponent::~CWeaponPickupComponent()
{
}

void CWeaponPickupComponent::SetWeaponPickup(const std::string& aWeaponName)
{
	myWeaponPickup = aWeaponName;
}

void CWeaponPickupComponent::DoMyEffect()
{
	SComponentMessageData data;
	data.myString = myWeaponPickup.c_str();
	CPollingStation::GetInstance()->GetPlayerObject()->NotifyComponents(eComponentMessageType::eAddWeapon,data);

	SAmmoReplenishData AmmoReplensihData;
	AmmoReplensihData.ammoType = myWeaponPickup.c_str();
	AmmoReplensihData.replenishAmount = 1000;
	SComponentMessageData giveAmmoData;
	giveAmmoData.myAmmoReplenishData = &AmmoReplensihData;
	CPollingStation::GetInstance()->GetPlayerObject()->NotifyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);

	CNetworkMessage_PickupWeapon* message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PickupWeapon>(ID_ALL);

	SComponentQuestionData questionData;
	SWeaponPickupData temporaryWeaponPickupData;
	questionData.myWeaponPickupData = &temporaryWeaponPickupData;
	questionData.myWeaponPickupData->weaponName = myWeaponPickup.c_str();
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetWeaponFactoryIndexOfSpecificWeapon, questionData);

	message->SetWeapon(questionData.myWeaponPickupData->weaponFactoryIndex);
	message->SetID(myNetworkId);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(message));

	if (myWeaponPickup == "PlasmaRifle")
	{
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"You picked up a Plasme Rifle"));
	}
	else if (myWeaponPickup == "BFG")
	{
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"You picked up the BFG"));
	}
	{
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"You picked up a " + CU::StringToWString(myWeaponPickup)));
	}
}
