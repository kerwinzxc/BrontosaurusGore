#pragma once
#include "NetworkMessage.h"

class CNetworkMessage_PingResponse: public CNetworkMessage
{
public:
	CNetworkMessage_PingResponse();
	~CNetworkMessage_PingResponse();
};

