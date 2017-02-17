#pragma once
#include "NetworkMessage.h"

class CNetworkMessage_Connect:public CNetworkMessage
{
public:
	CNetworkMessage_Connect();
	~CNetworkMessage_Connect();

	std::string myClientName;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

