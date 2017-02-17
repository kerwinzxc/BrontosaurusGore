#pragma once
#include <string>
#include "NetworkMessage.h"

class CNetworkMessage_ChatMessage:public CNetworkMessage
{
public:
	CNetworkMessage_ChatMessage();
	~CNetworkMessage_ChatMessage();

	std::string myChatMessage;
private:

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

