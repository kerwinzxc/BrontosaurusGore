#pragma once
#include "NetworkMessage.h"

class CNetworkMessage_Ping: public CNetworkMessage
{
public:
	friend class CMessageStorage;
	
	CNetworkMessage_Ping();
	~CNetworkMessage_Ping();

	ePackageType GetPackageType()const override;

private:

};

