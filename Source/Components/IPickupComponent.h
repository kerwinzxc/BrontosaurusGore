#pragma once
#include "Component.h"
class IPickupComponent : public CComponent
{
public:
	IPickupComponent();
	~IPickupComponent();

	virtual void ReInit();

	void SetActive(const bool aFlag);
	void SetNetworkId(const int aID);

	const bool GetIsActive() const;
	const int GetNetworkId() const;

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	virtual const bool GetShouldReset() const;

	virtual void DoMyEffect() = 0;
protected:
	int myNetworkId;
private:
	bool myHasBeenPickedUp;
};

