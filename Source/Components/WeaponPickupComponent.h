#pragma once
#include "IPickupComponent.h"
class CWeaponPickupComponent : public IPickupComponent
{
public:
	CWeaponPickupComponent();
	~CWeaponPickupComponent();

	void SetWeaponPickup(const std::string& aWeaponName);

	virtual void DoMyEffect() override;
private:
	std::string myWeaponPickup;
};

