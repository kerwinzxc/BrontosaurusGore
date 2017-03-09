#pragma once
#include "Component.h"
#include "HealthPoint.h"
class CHealthPackComponent :
	public CComponent
{
public:
	CHealthPackComponent();
	~CHealthPackComponent();
	void SetRestoreAmount(const healthPoint aResoreAmount);

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
private:
	void RestoreHealth();
private:
	healthPoint myRestoreAmount;
};

