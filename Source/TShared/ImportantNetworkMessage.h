#pragma once
#include "NetworkMessage.h"


class CImportantNetworkMessage: public CNetworkMessage
{
public:
	CImportantNetworkMessage();
	~CImportantNetworkMessage();

	bool IsImportant() override;

};

