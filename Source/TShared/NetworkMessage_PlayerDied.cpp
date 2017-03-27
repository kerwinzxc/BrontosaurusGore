#include "stdafx.h"
#include "NetworkMessage_PlayerDied.h"


CNetworkMessage_PlayerDied::CNetworkMessage_PlayerDied()
{
}


CNetworkMessage_PlayerDied::~CNetworkMessage_PlayerDied()
{
}

ePackageType CNetworkMessage_PlayerDied::GetPackageType() const
{
	return ePackageType::ePlayerDied;
}

void CNetworkMessage_PlayerDied::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
}

void CNetworkMessage_PlayerDied::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
}
