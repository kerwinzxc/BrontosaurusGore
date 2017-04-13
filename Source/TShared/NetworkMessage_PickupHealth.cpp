#include "stdafx.h"
#include "NetworkMessage_PickupHealth.h"


CNetworkMessage_PickupHealth::CNetworkMessage_PickupHealth()
{
	myNetworkID = -1;
}


CNetworkMessage_PickupHealth::~CNetworkMessage_PickupHealth()
{
}

ePackageType CNetworkMessage_PickupHealth::GetPackageType() const
{
	return ePackageType::ePickupHealth;
}

const int CNetworkMessage_PickupHealth::GetID()
{
	return myNetworkID;
}

void CNetworkMessage_PickupHealth::SetID(const int aId)
{
	myNetworkID = aId;
}

void CNetworkMessage_PickupHealth::SetReplenishAmount(const short aValue)
{
	myReplenishAmount = aValue;
}
const short CNetworkMessage_PickupHealth::GetReplenishAmount() const
{
	return myReplenishAmount;
}

void CNetworkMessage_PickupHealth::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myNetworkID, aStream);
	serialize(myReplenishAmount, aStream);
}

void CNetworkMessage_PickupHealth::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myNetworkID = deserialize<int>(aStream);
	myReplenishAmount = deserialize<short>(aStream);
}
