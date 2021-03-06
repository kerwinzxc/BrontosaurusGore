#include "stdafx.h"
#include "PlayerNetworkComponent.h"
#include "../ThreadedPostmaster/SetClientIDMessage.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../TShared/NetworkMessage_PlayerPositionMessage.h"
#include "../TClient/ClientMessageManager.h"
#include "../ThreadedPostmaster/PlayerPositionMessage.h"
#include "../TShared/NetworkMessage_PlayerDied.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../tshared/NetworkMessage_PlayerRespawned.h"



CPlayerNetworkComponent::CPlayerNetworkComponent() : myID(0)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSetClientIDMessage(*this));
	myIsActive = true;
}


CPlayerNetworkComponent::~CPlayerNetworkComponent()
{
}

void CPlayerNetworkComponent::SetID(const unsigned aID)
{
	myID = aID;
}

void CPlayerNetworkComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{

	switch (aMessageType)
	{
	case eComponentMessageType::eMoving:
	{
		SComponentQuestionData data;
		CU::Matrix44f transform = GetParent()->GetToWorldTransform();
		transform.GetPosition().y += 1;
		/*	if (GetParent()->AskComponents(eComponentQuestionType::eGetCameraObject, data) == true)
			{
				transform = data.myGameObject->GetLocalTransform() * GetParent()->GetToWorldTransform();
			}*/



		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerPositionMessage(transform, myID));
	}
	break;
	case eComponentMessageType::eDied:
	{
		CNetworkMessage_PlayerDied* playerDied = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PlayerDied>(ID_ALL);

		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(playerDied));
		myIsActive = false;
	}
	break;
	case eComponentMessageType::eCheckPointReset:
	{
		if(myIsActive == false)
		{
			CNetworkMessage_PlayerRespawned* playerRespawned = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PlayerRespawned>(ID_ALL);

			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(playerRespawned));
			myIsActive = true;
			DL_PRINT("Back");
		}
	}
	break;
	default: break;
	}


}
