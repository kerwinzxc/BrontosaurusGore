#pragma once
#include "Component.h"
class CArmorPickup : public CComponent
{
public:
	CArmorPickup();
	~CArmorPickup();
	void SetArmorAmount(const unsigned short aValue);
private:
	unsigned short myReplenishAmount;
};

