#pragma once
#include "ImportantNetworkMessage.h"

class CNetworkMessage_ServerReady : public CImportantNetworkMessage
{
public:
	friend class CMessageManager;

	CNetworkMessage_ServerReady();
	~CNetworkMessage_ServerReady();

	ePackageType GetPackageType()const override;

	unsigned GetNumberOfPlayers();

	void SetNumberOfPlayers(unsigned aNumberOfPlayers);

private:

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

	unsigned myNumberOfPlayersToWaitFor;
};

