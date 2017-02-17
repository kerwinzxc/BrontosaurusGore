#include "stdafx.h"
#include "NetworkMessage_ChatMessage.h"


CNetworkMessage_ChatMessage::CNetworkMessage_ChatMessage()
{
}


CNetworkMessage_ChatMessage::~CNetworkMessage_ChatMessage()
{
}

void CNetworkMessage_ChatMessage::DoSerialize(StreamType& aStream)
{
	serialize(myChatMessage, aStream);
}

void CNetworkMessage_ChatMessage::DoDeserialize(StreamType& aStream)
{
	myChatMessage = deserialize<std::string>(aStream);
}
