#pragma once
#include "Component.h"
#include "IPickupComponent.h"
#include "AmmoReplenishData.h"

class CPickupAmmoComponent : public IPickupComponent
{
public:
	CPickupAmmoComponent();
	~CPickupAmmoComponent();

	void SetType(const std::string& aType);
	void SetPickupAmount(const unsigned short aAmount);

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
	void DoMyEffect() override;
private:
	void GiveAmmoType();

	SAmmoReplenishData myPickupData;

	// Inherited via IPickupComponent
};

