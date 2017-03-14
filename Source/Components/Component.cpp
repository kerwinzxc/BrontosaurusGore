#include "stdafx.h"
#include "Component.h"

CComponent::CComponent()
	: myType(eComponentType::eNone)
	, myParent(nullptr)
	, myId(NULL_COMPONENT)
{
}

CComponent::~CComponent()
{
}

void CComponent::NotifyParent(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	if (myParent != nullptr)
	{
		myParent->NotifyComponents(aMessageType, aMessageData);
	}
}

void CComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	aMessageType;
	aMessageData;
}

bool CComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	aQuestionType;
	aQuestionData;

	return false;
}

bool CComponent::IsGameObject()
{
	return false;
}
