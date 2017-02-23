#pragma once
#include "Component.h"

struct AmmoData;
class AmmoComponent : public CComponent
{
public:
	AmmoComponent();
	~AmmoComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
	void Update(float aDeltaTime);
private:
	AmmoData* myAmmoData;
	unsigned short myCurrentAmmo;
};

