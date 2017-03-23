#include "stdafx.h"
#include "NetworkPlayerReciverComponent.h"
#include "../CommonUtilities/JsonValue.h"

CNetworkPlayerReciverComponent::CNetworkPlayerReciverComponent()
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
	GetParent()->GetLocalTransform().LerpPosition(myInterpolationPosition, myInterpolationSpeed*aDeltaTime.GetSeconds());
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}

void CNetworkPlayerReciverComponent::SetInpolationPosition(const CU::Vector3f & aPosition)
{
	myInterpolationPosition = aPosition;
}
