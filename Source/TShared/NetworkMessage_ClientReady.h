#pragma once
#include "ImportantNetworkMessage.h"

class CNetworkMessage_ClientReady :public CImportantNetworkMessage
{
public:
	CNetworkMessage_ClientReady();
	~CNetworkMessage_ClientReady();

	ePackageType GetPackageType()const override;

private:
};

