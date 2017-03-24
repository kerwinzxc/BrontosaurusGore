#pragma once
#include "ImportantNetworkMessage.h"
class CNetworkMessage_RevivePlayer : public CImportantNetworkMessage
{
public:
	CNetworkMessage_RevivePlayer();
	~CNetworkMessage_RevivePlayer();

	ePackageType GetPackageType()const override;

private:

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

