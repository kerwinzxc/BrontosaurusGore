#pragma once
#include "ImportantNetworkMessage.h"

class CNetworkMessage_PlayerDied:public CImportantNetworkMessage
{
public:
	CNetworkMessage_PlayerDied();
	~CNetworkMessage_PlayerDied();

	ePackageType GetPackageType()const override;

private:

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

};

