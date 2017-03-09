#include "stdafx.h"
#include "HealthPackComponent.h"


CHealthPackComponent::CHealthPackComponent()
{
	myRestoreAmount = 0;
}


CHealthPackComponent::~CHealthPackComponent()
{
}

void CHealthPackComponent::SetRestoreAmount(const healthPoint aResoreAmount)
{
	myRestoreAmount = aResoreAmount;
}

void CHealthPackComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
		//Again needs collision
	default:
		break;
	}
}

bool CHealthPackComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	return false;
}

void CHealthPackComponent::RestoreHealth()
{
	SComponentMessageData data;
	data.myInt;
	GetParent()->NotifyComponents(eComponentMessageType::eHeal, data);
}
