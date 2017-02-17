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
	float myElapsedRealodingTime;
	unsigned short myCurrentAmmoInClip;
	unsigned short myCurrentAmmo;

	bool myIsReloding;
};
