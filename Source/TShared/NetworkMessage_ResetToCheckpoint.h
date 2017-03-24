#pragma once
#include "ImportantNetworkMessage.h"
class CNetworkMessage_ResetToCheckpoint: public CImportantNetworkMessage
{
public:
	CNetworkMessage_ResetToCheckpoint();
	~CNetworkMessage_ResetToCheckpoint();

	ePackageType GetPackageType()const override;

private:

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

