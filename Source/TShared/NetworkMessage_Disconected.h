#pragma once
#include "NetworkMessage.h"

class CNetworkMessage_Disconected:public CNetworkMessage
{
public:
	CNetworkMessage_Disconected();
	~CNetworkMessage_Disconected();

	ePackageType GetPackageType()const override;


private:

	std::string myClientName;

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

