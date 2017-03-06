#pragma once
#include "Component.h"

struct SGeneralAmmoData; //Names are hard :/
class CAmmoComponent : public CComponent
{
public:
	CAmmoComponent();
	~CAmmoComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
	void Update(float aDeltaTime);

private:
	void ChangeSelectedAmmoType(const char* aAmmoType);
private:
	CU::GrowingArray<SGeneralAmmoData*> myGeneralAmmoDataList;
	unsigned int mySelectedAmmoType;
};

