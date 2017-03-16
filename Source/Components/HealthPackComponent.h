#pragma once
#include "IPickupComponent.h"
#include "HealthPoint.h"
class CHealthPackComponent : public IPickupComponent
{
public:
	CHealthPackComponent();
	~CHealthPackComponent();
	void SetRestoreAmount(const healthPoint aResoreAmount);

	void DoMyEffect() override;

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
private:
	void RestoreHealth();
private:
	healthPoint myRestoreAmount;
	bool myHasBeenPickedUp;

	// Inherited via IPickupComponent
};

