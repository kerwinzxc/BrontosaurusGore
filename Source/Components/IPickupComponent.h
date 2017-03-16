#pragma once
#include "Component.h"
class IPickupComponent : public CComponent
{
public:
	IPickupComponent();
	~IPickupComponent();

	void SetActive(const bool aFlag);
	void SetNetworkId(const int aID);

	const bool GetIsActive() const;
	const int GetNetworkId() const;

	virtual void DoMyEffect() = 0;
protected:
	bool myHasBeenPickedUp;
	int myNetworkId;
};
