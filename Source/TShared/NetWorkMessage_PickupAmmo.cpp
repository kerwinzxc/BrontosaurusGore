#include "stdafx.h"
#include "NetWorkMessage_PickupAmmo.h"


CNetWorkMessage_PickupAmmo::CNetWorkMessage_PickupAmmo()
{
	myNetworkID = -1;
}


CNetWorkMessage_PickupAmmo::~CNetWorkMessage_PickupAmmo()
{
}

ePackageType CNetWorkMessage_PickupAmmo::GetPackageType() const
{
	return ePackageType::ePickupAmmo;
}

const int CNetWorkMessage_PickupAmmo::GetID()
{
	return myNetworkID;
}

void CNetWorkMessage_PickupAmmo::SetID(const int aId)
{
	myNetworkID = aId;
}

void CNetWorkMessage_PickupAmmo::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myNetworkID, aStream);
}

void CNetWorkMessage_PickupAmmo::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myNetworkID = deserialize<int>(aStream);
}
