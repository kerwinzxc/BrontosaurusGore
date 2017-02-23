#pragma once
#include "Component.h"

struct GeneralAmmoData; //Names are hard :/
class AmmoComponent : public CComponent
{
public:
	AmmoComponent();
	~AmmoComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
	void Update(float aDeltaTime);
private:
	CU::GrowingArray<GeneralAmmoData*> myGeneralAmmoDataList;
	unsigned int mySelectedAmmoType;
};

