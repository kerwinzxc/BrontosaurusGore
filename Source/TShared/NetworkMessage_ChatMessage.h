#pragma once
#include <string>
#include "NetworkMessage.h"

class CNetworkMessage_ChatMessage:public CNetworkMessage
{
public:
	friend class CMessageStorage;

	CNetworkMessage_ChatMessage();
	~CNetworkMessage_ChatMessage();

	ePackageType GetPackageType()const override;

	std::string myChatMessage;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

