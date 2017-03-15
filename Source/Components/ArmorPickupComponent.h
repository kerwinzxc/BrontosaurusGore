#pragma once
#include "HealthPoint.h"
#include "IPickupComponent.h"

class CArmorPickupComponent : public IPickupComponent
{
public:
	CArmorPickupComponent();
	~CArmorPickupComponent();

	void DoMyEffect() override;
private:
	armorPoint myReplenishAmount;
};

