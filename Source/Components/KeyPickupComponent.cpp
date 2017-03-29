#include "stdafx.h"
#include "KeyPickupComponent.h"
#include "PollingStation.h"
#include "..\TClient\ClientMessageManager.h"
#include "..\TShared\NetworkMessage_PickupKey.h"
#include "..\ThreadedPostmaster\Postmaster.h"
#include "..\ThreadedPostmaster\SendNetowrkMessageMessage.h"

CKeyPickupComponent::CKeyPickupComponent()
{
	myLockId = -1;
	myType = eComponentType::eKeyPickup;
}


CKeyPickupComponent::~CKeyPickupComponent()
{
}

const bool CKeyPickupComponent::GetShouldReset() const
{
	return false;
}

void CKeyPickupComponent::SetLockId(const lockID alockId)
{
	myLockId = alockId;
}

void CKeyPickupComponent::DoMyEffect()
{
	SetActive(false);
	CPollingStation::GetInstance()->AddKey(myLockId);
	CNetworkMessage_PickupKey* message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PickupKey>(ID_ALL_BUT_ME);
	message->SetID(myNetworkId);
	message->SetLockID(myLockId);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(message));
}
