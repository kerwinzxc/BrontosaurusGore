#include "stdafx.h"
#include "NetworkMessage_LoadLevel.h"


CNetworkMessage_LoadLevel::CNetworkMessage_LoadLevel(): myLevelIndex(0)
{
	myHeader.myPackageType = (ePackageType::eLoadLevel);
}


CNetworkMessage_LoadLevel::~CNetworkMessage_LoadLevel()
{
}

ePackageType CNetworkMessage_LoadLevel::GetPackageType() const
{
	return ePackageType::eLoadLevel;
}

void CNetworkMessage_LoadLevel::DoSerialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myLevelIndex, aStream);
}

void CNetworkMessage_LoadLevel::DoDeserialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myLevelIndex = deserialize<int>(aStream);
}

