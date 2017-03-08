#include "stdafx.h"
#include "NetworkPlayerReciverComponent.h"


CNetworkPlayerReciverComponent::CNetworkPlayerReciverComponent()
{
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
