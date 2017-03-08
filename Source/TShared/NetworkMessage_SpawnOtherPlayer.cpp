#include "stdafx.h"
#include "NetworkMessage_SpawnOtherPlayer.h"


CNetworkMessage_SpawnOtherPlayer::CNetworkMessage_SpawnOtherPlayer(): myPlayerID(0)
{
}


CNetworkMessage_SpawnOtherPlayer::~CNetworkMessage_SpawnOtherPlayer()
{
}

void CNetworkMessage_SpawnOtherPlayer::SetPlayerId(const unsigned aPlayerID)
{
	myPlayerID = aPlayerID;
}

unsigned CNetworkMessage_SpawnOtherPlayer::GetPlayerID() const
{
	return myPlayerID;
}

ePackageType CNetworkMessage_SpawnOtherPlayer::GetPackageType() const
{
	return ePackageType::eSpawnOtherPlayer;
}

void CNetworkMessage_SpawnOtherPlayer::DoSerialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myPlayerID, aStream);
}

void CNetworkMessage_SpawnOtherPlayer::DoDeserialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myPlayerID = deserialize<decltype(myPlayerID)>(aStream);
}
