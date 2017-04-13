#include "stdafx.h"
#include "ShowTitleComponent.h"
#include "ComponentManager.h"

CShowTitleComponent::CShowTitleComponent()
{
	if (CComponentManager::GetInstancePtr())
	{
		CComponentManager::GetInstance().RegisterComponent(this);
	}
	myAlpha = -1.0f;
	myFadeIn = false; 
	myCurrentIndex = -1;
}


CShowTitleComponent::~CShowTitleComponent()
{
}


void CShowTitleComponent::Update(const float aDT)
{
	if (myFadeIn)
	{
		if(myAlpha < 1.0f)
			myAlpha += 3.5 * aDT;
	}
	else
	{
		if (myAlpha > 0.0f)
			myAlpha -= 1.5 * aDT;
	}
	CLAMP(myAlpha, 0.0f, 1.0f);
}

void CShowTitleComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnTriggerEnter:
		myFadeIn = true;
		myCurrentIndex++;
		DL_PRINT("INDEX:%d", myCurrentIndex);
		break;
	case eComponentMessageType::eOnTriggerExit:
		myFadeIn = false;
		break;
	}
}

