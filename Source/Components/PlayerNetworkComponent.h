#pragma once
#include "Component.h"
class CPlayerNetworkComponent: public CComponent
{
public:
	CPlayerNetworkComponent(const unsigned aID);
	~CPlayerNetworkComponent();

private:
	unsigned myID;
};

