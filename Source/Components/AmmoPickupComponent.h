#pragma once
#include "Component.h"
#include "IPickupComponent.h"
#include "AmmoReplenishData.h"

class CAmmoPickupComponent : public IPickupComponent
{
public:
	CAmmoPickupComponent();
	~CAmmoPickupComponent();

	void SetType(const std::string& aType);
	void SetPickupAmount(const unsigned short aAmount);

	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
	void DoMyEffect() override;
private:
	void GiveAmmoType();

	SAmmoReplenishData myPickupData;

	// Inherited via IPickupComponent
};

