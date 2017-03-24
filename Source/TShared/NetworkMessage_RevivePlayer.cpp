#include "stdafx.h"
#include "NetworkMessage_RevivePlayer.h"


CNetworkMessage_RevivePlayer::CNetworkMessage_RevivePlayer()
{
}


CNetworkMessage_RevivePlayer::~CNetworkMessage_RevivePlayer()
{
}

ePackageType CNetworkMessage_RevivePlayer::GetPackageType() const
{
	return ePackageType::eRevivePlayer;
}

void CNetworkMessage_RevivePlayer::DoSerialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
}

void CNetworkMessage_RevivePlayer::DoDeserialize(StreamType & aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
}
