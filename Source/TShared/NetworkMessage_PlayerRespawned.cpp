#include "stdafx.h"
#include "NetworkMessage_PlayerRespawned.h"


CNetworkMessage_PlayerRespawned::CNetworkMessage_PlayerRespawned()
{
}


CNetworkMessage_PlayerRespawned::~CNetworkMessage_PlayerRespawned()
{
}

ePackageType CNetworkMessage_PlayerRespawned::GetPackageType() const
{
	return ePackageType::ePlayerRespawned;
}

void CNetworkMessage_PlayerRespawned::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
}

void CNetworkMessage_PlayerRespawned::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
}
