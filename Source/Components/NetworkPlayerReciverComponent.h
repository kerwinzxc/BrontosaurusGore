#pragma once
#include "../Components/Component.h"

class CNetworkPlayerReciverComponent: public CComponent
{
public:
	CNetworkPlayerReciverComponent();
	~CNetworkPlayerReciverComponent();

	unsigned GetPlayerID();
	void SetPlayerID(const unsigned anID);

private:
	unsigned myPlayerId;
};

