#include "stdafx.h"
#include "WeaponPickupComponent.h"
#include "PollingStation.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../TShared/NetworkMessage_PickupWeapon.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TClient/ClientMessageManager.h"

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
	CPollingStation::GetInstance()->GetPlayerObject()->NotifyComponents(eComponentMessageType::eAddWeapon, data);
	CNetworkMessage_PickupWeapon* message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PickupWeapon>(ID_ALL_BUT_ME);

	SComponentQuestionData data2;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetWeaponFactoryIndexOfActiveWeapon, data2);
	CPollingStation::GetInstance()->AddWeapon(data2.myInt);

	int id = data2.myInt;

	message->SetID(myNetworkId);
 	message->SetWeapon(data2.myInt);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(message));
}
