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

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	virtual void DoMyEffect() = 0;
protected:
	int myNetworkId;
private:
	bool myHasBeenPickedUp;
};

