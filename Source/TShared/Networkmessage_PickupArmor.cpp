#include "stdafx.h"
#include "Networkmessage_PickupArmor.h"


CNetworkmessage_PickupArmor::CNetworkmessage_PickupArmor()
{
	myNetworkID = -1;
}


CNetworkmessage_PickupArmor::~CNetworkmessage_PickupArmor()
{
}

ePackageType CNetworkmessage_PickupArmor::GetPackageType() const
{
	return ePackageType::ePickupArmor;
}

const int CNetworkmessage_PickupArmor::GetID()
{
	return myNetworkID;
}

void CNetworkmessage_PickupArmor::SetID(const int aId)
{
	myNetworkID = aId;
}

void CNetworkmessage_PickupArmor::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myNetworkID, aStream);
}

void CNetworkmessage_PickupArmor::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myNetworkID = deserialize<int>(aStream);
}
