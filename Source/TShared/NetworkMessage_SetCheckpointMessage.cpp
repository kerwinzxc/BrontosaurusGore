#include "stdafx.h"
#include "NetworkMessage_SetCheckpointMessage.h"


CNetworkMessage_SetCheckpointMessage::CNetworkMessage_SetCheckpointMessage()
{
}


CNetworkMessage_SetCheckpointMessage::~CNetworkMessage_SetCheckpointMessage()
{
}

void CNetworkMessage_SetCheckpointMessage::DoSerialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	SERIALIZE(myId, aStream);
}
void CNetworkMessage_SetCheckpointMessage::DoDeserialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	DESERIALIZE(myId, aStream);
}