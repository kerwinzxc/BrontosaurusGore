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
		//DL_PRINT("Object moving! sent from CNetworkComponent");

	switch (aMessageType)
	{
	case eComponentMessageType::eHeal:
	{
		CServerMessageManager* instance = CServerMessageManager::GetInstance();
		if (instance == nullptr)
		{
			//DL_PRINT("woop");
		}
		CU::Vector3f temp;
		if (instance != nullptr)
		{
			temp.z += static_cast<float>((rand() % 500 +1)/100.f);
			GetParent()->GetLocalTransform().Rotate(static_cast<float>(((rand() % 30 + 1) - 15) * (PI / 180.f)),static_cast<CU::Axees>(rand() % 3));
			GetParent()->GetLocalTransform().Move(temp);

		}
		CNetworkMessage_Position* positionMessage = instance->CreateMessage<CNetworkMessage_Position>(static_cast<unsigned>(ID_ALL));

		positionMessage->SetPosition(GetParent()->GetWorldPosition());
		positionMessage->SetID(myNetworkId);

		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(positionMessage));

	}
		break;
	}
}

bool CNetworkComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	return false;
}
