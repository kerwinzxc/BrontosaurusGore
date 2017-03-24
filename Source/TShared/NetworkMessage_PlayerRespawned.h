#pragma once
#include "../TShared/ImportantNetworkMessage.h"
class CNetworkMessage_PlayerRespawned: public CImportantNetworkMessage
{
public:
	CNetworkMessage_PlayerRespawned();
	~CNetworkMessage_PlayerRespawned();

	ePackageType GetPackageType()const override;

private:

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

