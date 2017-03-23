#include "stdafx.h"
#include "NetworkMessage_ResetToCheckpoint.h"


CNetworkMessage_ResetToCheckpoint::CNetworkMessage_ResetToCheckpoint()
{
}


CNetworkMessage_ResetToCheckpoint::~CNetworkMessage_ResetToCheckpoint()
{
}

ePackageType CNetworkMessage_ResetToCheckpoint::GetPackageType() const
{
	return ePackageType::eResetToCheckpoint;
}

void CNetworkMessage_ResetToCheckpoint::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
}

void CNetworkMessage_ResetToCheckpoint::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
}
