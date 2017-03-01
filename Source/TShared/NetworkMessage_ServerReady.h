#pragma once
#include "ImportantNetworkMessage.h"

class CNetworkMessage_ServerReady : public CImportantNetworkMessage
{
public:
	CNetworkMessage_ServerReady();
	~CNetworkMessage_ServerReady();

	ePackageType GetPackageType()const override;
};

