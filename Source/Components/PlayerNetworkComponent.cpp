#include "stdafx.h"
#include "PlayerNetworkComponent.h"
#include "../ThreadedPostmaster/SetClientIDMessage.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../TShared/NetworkMessage_PlayerPositionMessage.h"
#include "../TClient/ClientMessageManager.h"
#include "../ThreadedPostmaster/PlayerPositionMessage.h"


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
		SComponentQuestionData data;
		CU::Matrix44f transform = GetParent()->GetToWorldTransform();
		if (GetParent()->AskComponents(eComponentQuestionType::eGetCameraObject, data) == true)
		{
			transform = data.myGameObject->GetLocalTransform() * GetParent()->GetToWorldTransform();
		}

		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CPlayerPositionMessage(transform, myID));
	}
	break;
	default: break;
	}


}
