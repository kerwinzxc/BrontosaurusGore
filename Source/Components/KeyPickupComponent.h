#pragma once
#include "IPickupComponent.h"

typedef short lockID;

class CKeyPickupComponent : public IPickupComponent
{
public:
	CKeyPickupComponent();
	~CKeyPickupComponent();

	const bool GetShouldReset() const override;

	void SetLockId(const lockID alockId);
	void DoMyEffect() override;
private:
	lockID myLockId;
};

