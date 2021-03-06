#include "stdafx.h"
#include "NetworkPlayerReciverComponent.h"
#include "../CommonUtilities/JsonValue.h"
#include "PollingStation.h"

CNetworkPlayerReciverComponent::CNetworkPlayerReciverComponent(): myPlayerId(0)
{
	CU::CJsonValue playerControls;
	std::string errorMessage = playerControls.Parse("Json/Player/playerData.json");
	myInterpolationSpeed = playerControls["MaxSpeed"].GetFloat();
}


CNetworkPlayerReciverComponent::~CNetworkPlayerReciverComponent()
{
}

unsigned CNetworkPlayerReciverComponent::GetPlayerID()
{
	return myPlayerId;
}

void CNetworkPlayerReciverComponent::SetPlayerID(const unsigned anID)
{
	myPlayerId = anID;
}

void CNetworkPlayerReciverComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
}

void CNetworkPlayerReciverComponent::Update(const CU::Time aDeltaTime)
{
	if (CPollingStation::CheckInstance() == false)
	{
		return;
	}

	GetParent()->GetLocalTransform().LerpPosition(myInterpolationPosition, myInterpolationSpeed*aDeltaTime.GetSeconds());
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}

void CNetworkPlayerReciverComponent::SetInpolationPosition(const CU::Vector3f & aPosition)
{
	myInterpolationPosition = aPosition;
}
