#include "stdafx.h"
#include "NetworkMessage_PickupKey.h"


CNetworkMessage_PickupKey::CNetworkMessage_PickupKey()
{
	myLockID = 0;
	myNetWorkId = -1;
}


CNetworkMessage_PickupKey::~CNetworkMessage_PickupKey()
{
}

ePackageType CNetworkMessage_PickupKey::GetPackageType() const
{
	return ePackageType::ePickupKey;
}



void CNetworkMessage_PickupKey::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myLockID, aStream);
	serialize(myNetWorkId, aStream);
}

void CNetworkMessage_PickupKey::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myLockID = deserialize<short>(aStream);
	myNetWorkId = deserialize<int>(aStream);
}

void CNetworkMessage_PickupKey::SetID(const int aId)
{
	myNetWorkId = aId;
}

const int CNetworkMessage_PickupKey::GetNetWorkID() const
{
	return myNetWorkId;
}

void CNetworkMessage_PickupKey::SetLockID(const short aLockID)
{
	myLockID = aLockID;
}

const short CNetworkMessage_PickupKey::GetLockID() const
{
	return myLockID;
}
