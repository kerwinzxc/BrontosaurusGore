#include "stdafx.h"
#include "NetworkComponent.h"
#include "../TShared/NetworkMessage_Position.h"
#include "../PostMaster/Message.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PostOffice.h"
#include "../TServer/ServerMessageManager.h"


CNetworkComponent::CNetworkComponent(NetworkId aNetworkId): myNetworkId(aNetworkId)
{
	myType = eComponentType::eNetwork;
}


CNetworkComponent::~CNetworkComponent()
{
}

void CNetworkComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
		DL_PRINT("Object moving! sent from CNetworkComponent");
		CServerMessageManager* instance = CServerMessageManager::GetInstance();
		if (instance == nullptr)
		{
			DL_PRINT("woop");
		}
		CNetworkMessage_Position* positionMessage = instance->CreateMessage<CNetworkMessage_Position>(static_cast<unsigned>(ID_ALL));

		positionMessage->SetPosition(GetParent()->GetWorldPosition());
		positionMessage->SetID(myNetworkId);

		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(positionMessage));

	switch (aMessageType)
	{
	case eComponentMessageType::eMoving:
	{

	}
		break;
	}
}

bool CNetworkComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	return false;
}
