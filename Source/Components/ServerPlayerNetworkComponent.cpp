#include "stdafx.h"
#include "ServerPlayerNetworkComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TServer/ServerMessageManager.h"
#include "../TShared/NetworkMessage_TakeDamage.h"


CServerPlayerNetworkComponent::CServerPlayerNetworkComponent()
{
}

CServerPlayerNetworkComponent::~CServerPlayerNetworkComponent()
{
}

void CServerPlayerNetworkComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eTakeDamage:
	{
		CNetworkMessage_TakeDamage* takeDamage = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_TakeDamage>(myClientID);
		takeDamage->SetDamageTaken(aMessageData.myInt);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(takeDamage));
		DL_PRINT("TookDamage on server");
	}
	}
}

void CServerPlayerNetworkComponent::SetClientID(const unsigned short aClientID)
{
	myClientID = aClientID;
}

void CServerPlayerNetworkComponent::Update()
{
	SComponentMessageData controllerPositionData;
	controllerPositionData.myVector3f = GetParent()->GetLocalTransform().GetPosition();
	GetParent()->NotifyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
}