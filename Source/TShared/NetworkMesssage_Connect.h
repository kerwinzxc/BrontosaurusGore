#pragma once
#include "ImportantNetworkMessage.h"

class CNetworkMessage_Connect:public CImportantNetworkMessage
{
public:
	friend class CMessageStorage;
	CNetworkMessage_Connect();
	~CNetworkMessage_Connect();

	ePackageType GetPackageType()const override;
	
	std::string myClientName;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

