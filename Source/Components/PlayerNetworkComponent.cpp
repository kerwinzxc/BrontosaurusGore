#include "stdafx.h"
#include "PlayerNetworkComponent.h"
#include "../ThreadedPostmaster/SetClientIDMessage.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../TShared/NetworkMessage_PlayerPositionMessage.h"
#include "../TClient/ClientMessageManager.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"


CPlayerNetworkComponent::CPlayerNetworkComponent() : myID(0)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSetClientIDMessage(*this));
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
		CClientMessageManager* instance = CClientMessageManager::GetInstance();
		if (instance == nullptr)
		{
			//DL_PRINT("woop");
		}

	//	CNetworkMessage_PlayerPositionMessage * positionMessage = instance->CreateMessage<CNetworkMessage_PlayerPositionMessage>(static_cast<unsigned>(ID_ALL));

		//positionMessage->SetPosition(GetParent()->GetWorldPosition());
		//positionMessage->SetID(myID);

		//Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(positionMessage));

	}
	break;
	}


}
