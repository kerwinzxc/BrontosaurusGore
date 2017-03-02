#pragma once
#include "ImportantNetworkMessage.h"

class CNetworkMessage_ClientReady :public CImportantNetworkMessage
{
public:
	friend class CMessageManager;
	friend CNetworkMessage;

	CNetworkMessage_ClientReady();
	~CNetworkMessage_ClientReady();

	ePackageType GetPackageType()const override;

};

