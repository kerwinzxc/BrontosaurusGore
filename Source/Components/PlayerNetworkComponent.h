#pragma once
#include "Component.h"
class CPlayerNetworkComponent: public CComponent
{
public:
	CPlayerNetworkComponent();
	~CPlayerNetworkComponent();

private:
	unsigned myID;
};

