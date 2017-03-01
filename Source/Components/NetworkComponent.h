#pragma once
#include "../TShared/Tshared_NetworkId.h"

class CNetworkComponent: CComponent
{
public:
	CNetworkComponent(NetworkId aNetworkId);
	~CNetworkComponent();

private:
	const NetworkId myNetworkId;
};

