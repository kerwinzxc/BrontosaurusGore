#pragma once
#include "ImportantNetworkMessage.h"

class CNetworkMessage_LoadLevel :public CImportantNetworkMessage
{
public:
	friend class CMessageStorage;
	friend CNetworkMessage;

	CNetworkMessage_LoadLevel();
	~CNetworkMessage_LoadLevel();

	ePackageType GetPackageType()const override;

	int myLevelIndex;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

