#include "stdafx.h"
#include "AnimationComponent.h"
#include "ModelComponent.h"
#include "AnimationComponentLoader.h"

#include "../CommonUtilities/JsonValue.h"
#include "../TShared/AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

#define NU_HAR_JAG_DRAMATISERAT_KLART_MIN_DÖD 3.14f

CU::GrowingArray<CAnimationComponent*> CAnimationComponent::ourAnimations(64u);

CAnimationComponent::CAnimationComponent(CModelComponent& aModelCompoent)
	: myAnimationStack(4u)
	, myModelComponent(aModelCompoent)
{
	myType = eComponentType::eAnimationComponent;

	AnimationComponentLoader::LoadAnimations(myModelComponent, myAnimationStates);

	PushAnimation(myAnimationStates["idle"]);

	ourAnimations.Add(this);
}

CAnimationComponent::~CAnimationComponent()
{
	ourAnimations.RemoveCyclic(this);
}

void CAnimationComponent::Update(const CU::Time aDeltaTime)
{
	if (myAnimationStack.Empty())
	{
		return;
	}

	if (myAnimationStack.GetLast().myBlendTime > 0.f)
	{
		if (myAnimationStack.GetLast().myBlendTime < myAnimationStack.GetLast().myAnimationBlend)
		{
			myAnimationStack.GetLast().myAnimationBlend += aDeltaTime.GetSeconds();
		}
	}

	myModelComponent.SetAnimation(myAnimationStack.GetLast().myAnimationKey);
	myModelComponent.SetNextAnimation(myAnimationStack.GetLast().myNextAnimationKey);
	myModelComponent.SetAnimationLerpValue(myAnimationStack.GetLast().myAnimationBlend);
	myModelComponent.SetAnimationLooping(myAnimationStack.GetLast().myIsLooping);


	if (myAnimationStack.GetLast().myCoolDownTime != -1.f)
	{
		myAnimationStack.GetLast().myCoolDownTime -= aDeltaTime.GetSeconds();
		if (myAnimationStack.GetLast().myCoolDownTime <= 0.f)
		{
			myAnimationStack.Pop();
		}
	}
}

void CAnimationComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	aMessageData;

	if (myAnimationStates.empty()) return;
	auto it = myAnimationStates.begin();
	auto animationEnd = myAnimationStates.end();

	switch (aMessageType)
	{
	case eComponentMessageType::eShoot:
	case eComponentMessageType::eShootWithNetworking:
		if (myAnimationStack.GetLast().myAnimationKey == eAnimationState::shot01)
		{
			if (myAnimationStack.GetLast().myIsLooping)
			{
				myAnimationStack.GetLast().myCoolDownTime = myAnimationStates["shoot"].myCoolDownTime;
			}
		}
		else
		{
			PushAnimation(myAnimationStates["shoot"]);
			//myAnimationStack.Add(myAnimationStates["shoot"]);
			//myModelComponent.ResetAnimation();
		}
		break;
	case eComponentMessageType::eDied:
		it = myAnimationStates.find("death");
		if (it != animationEnd)
		{
			PushAnimation(it->second);
			//myAnimationStack.Add(it->second);
			//myModelComponent.ResetAnimation();
		}
	case eComponentMessageType::eSetVisibility:
		if (aMessageData.myBool == true)
		{
			it = myAnimationStates.find("equip");
			if (it != animationEnd)
			{
				PushAnimation(it->second);
				//myAnimationStack.Add(it->second);
				//myModelComponent.ResetAnimation();
			}
		}
		break;
	}
}

bool CAnimationComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	if (aQuestionType == eComponentQuestionType::eHasDeathAnimation)
	{
		auto it = myAnimationStates.find("death");
		if (it != myAnimationStates.end())
		{
			myAnimationStack.Add(it->second); //bugg??? const member function???
			return true;
		}
	}

	return false;
}

void CAnimationComponent::UpdateAnimations(const CU::Time aDeltaTime)
{
	for (CAnimationComponent* animationComponent : ourAnimations)
	{
		animationComponent->Update(aDeltaTime);
	}
}

void CAnimationComponent::PushAnimation(const SAnimation& aAnimation)
{
	myAnimationStack.Add(aAnimation);
	myModelComponent.ResetAnimation();
}
