#pragma once
#include "NetworkMessage.h"

class CNetworkMessage_ConectResponse: public CNetworkMessage
{
public:
	CNetworkMessage_ConectResponse();
	~CNetworkMessage_ConectResponse();

	short myClientId;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

