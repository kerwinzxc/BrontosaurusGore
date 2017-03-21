#pragma once
#include "HealthPoint.h"
#include "IPickupComponent.h"

class CArmorPickupComponent : public IPickupComponent
{
public:
	CArmorPickupComponent();
	~CArmorPickupComponent();

	void SetReplenishAmount(const armorPoint aAmount);

	void DoMyEffect() override;
private:
	armorPoint myReplenishAmount;
};

