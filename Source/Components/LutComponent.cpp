#include "stdafx.h"
#include "LutComponent.h"
#include "ComponentManager.h"

#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/Renderer.h"
#include "../BrontosaurusEngine/RenderMessages.h"
#include "../BrontosaurusEngine/ELUTType.h"
#include <PlayerControls.h>


CLutComponent::CLutComponent()
{
	if (CComponentManager::GetInstancePtr())
	{
		CComponentManager::GetInstance().RegisterComponent(this);
	}

	myFadeTimer = 0.0f;
	myDefaultFade.myFadeFrom = eDefault;
	myDefaultFade.myFadeTo = eDefault;
	myDefaultFade.myFadeTime = 0.5f;
}

CLutComponent::~CLutComponent()
{
}

void CLutComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eTookDamage:
		PushFade(eHurt, 0.1f, true);
		PushFade(eDefault, 0.1f, false);
		break;
	}
}

void CLutComponent::Update(const float aDeltaTime)
{
	SColorFade* currentFade = &myDefaultFade;

	myFadeTimer += aDeltaTime;
	if (myFadeTimer >= currentFade->myFadeTime)
	{
		PopFade();
	}

	if (!myFadeQueue.empty())
	{
		currentFade = &myFadeQueue.front();
	}

	CRenderer& renderer = RENDERER;
	SLutFadeColorGrade message;
	message.myFadeTime = myFadeTimer / currentFade->myFadeTime;
	message.myFadeFrom = currentFade->myFadeFrom;
	message.myFadeTo = currentFade->myFadeTo;
	renderer.AddRenderMessage(new SLutFadeColorGrade(message));
}

void CLutComponent::PushFade(const ELUTType aFadeTo, const float aTime, const bool aInterrupt)
{
	SColorFade fadeData;
	fadeData.myFadeFrom = (myFadeQueue.empty()) ? myDefaultFade.myFadeTo : myFadeQueue.back().myFadeTo;
	fadeData.myFadeTo = aFadeTo;
	fadeData.myFadeTime = aTime;

	if (aInterrupt && !myFadeQueue.empty())
	{
		float timePercentDone = myFadeTimer / myFadeQueue.front().myFadeTime;
		myFadeTimer = timePercentDone * aTime;

		//std::queue<SColorFade>().swap(myFadeQueue);
		while (!myFadeQueue.empty()) myFadeQueue.pop();
	}

	myFadeQueue.push(fadeData);
	DL_PRINT("pushed lut, size: %d", (int)myFadeQueue.size());
}

void CLutComponent::PopFade()
{
	if (!myFadeQueue.empty())
	{
		myFadeQueue.pop();
		DL_PRINT("popped lut, size: %d", (int)myFadeQueue.size());
	}

	myFadeTimer = 0.f;
}
