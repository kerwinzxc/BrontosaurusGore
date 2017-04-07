#include "stdafx.h"
#include "WeaponPickupComponent.h"
#include "PollingStation.h"
#include "../TShared/NetworkMessage_PickupWeapon.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../TClient/ClientMessageManager.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../ThreadedPostmaster/GameEventMessage.h"

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
	CNetworkMessage_PickupWeapon* message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PickupWeapon>(ID_ALL_BUT_ME);

	SComponentQuestionData questionData;
	CPollingStation::GetInstance()->GetPlayerObject()->AskComponents(eComponentQuestionType::eGetWeaponFactoryIndexOfActiveWeapon, questionData);

	message->SetWeapon(questionData.myInt);
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
