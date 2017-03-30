#include "stdafx.h"
#include "NetworkMessage_SetIsRepesentationActive.h"


CNetworkMessage_SetIsRepesentationActive::CNetworkMessage_SetIsRepesentationActive()
{
}


CNetworkMessage_SetIsRepesentationActive::~CNetworkMessage_SetIsRepesentationActive()
{
}

ePackageType CNetworkMessage_SetIsRepesentationActive::GetPackageType() const
{
	return ePackageType::eSetRepesentationActive;
}

void CNetworkMessage_SetIsRepesentationActive::SetActive(const bool aFlag)
{
	myIsActive = aFlag;
}

void CNetworkMessage_SetIsRepesentationActive::SetNetworkID(const unsigned short aID)
{
	myNetworkID = aID;
}

const bool CNetworkMessage_SetIsRepesentationActive::GetActive() const
{
	return myIsActive;
}

const unsigned short CNetworkMessage_SetIsRepesentationActive::GetNetworkID() const
{
	return myNetworkID;
}

void CNetworkMessage_SetIsRepesentationActive::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myIsActive, aStream);
	serialize(myNetworkID, aStream);
}

void CNetworkMessage_SetIsRepesentationActive::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	DESERIALIZE(myIsActive, aStream);
	DESERIALIZE(myNetworkID, aStream);
}
