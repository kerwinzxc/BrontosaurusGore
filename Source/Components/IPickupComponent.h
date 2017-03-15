#pragma once
#include "Component.h"
class IPickupComponent : public CComponent
{
public:
	IPickupComponent();
	~IPickupComponent();

	void SetActive(const bool aFlag);
	const bool GetIsActive() const;

	virtual void DoMyEffect() = 0;
protected:
	bool myHasBeenPickedUp;
};

