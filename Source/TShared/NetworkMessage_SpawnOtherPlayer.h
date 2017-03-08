#pragma once
#include "ImportantNetworkMessage.h"

class CNetworkMessage_SpawnOtherPlayer: public CImportantNetworkMessage
{
public:
	CNetworkMessage_SpawnOtherPlayer();
	~CNetworkMessage_SpawnOtherPlayer();

	void SetPlayerId(const unsigned aPlayerID);
	unsigned int GetPlayerID() const;

	ePackageType GetPackageType()const override;

private:
	unsigned int myPlayerID;

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

