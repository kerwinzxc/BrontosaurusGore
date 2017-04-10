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
	//, myUpdateHasRun(false)
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
	//myUpdateHasRun = true;
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

	CU::Vector3f positoin3D = GetParent()->GetToWorldTransform().GetPosition();
	CU::Vector2f position(positoin3D.x, positoin3D.z);
	CU::Vector2f velocity = position - myLastPosition;
	myLastPosition = position;

	//now we have the velocity, but not the boundaries for different velocities. lets get that on monday
	
	myModelComponent.SetAnimation(myAnimationStack.GetLast().myAnimationKey);
	myModelComponent.SetNextAnimation(myAnimationStack.GetLast().myNextAnimationKey);
	myModelComponent.SetAnimationLerpValue(myAnimationStack.GetLast().myAnimationBlend);
	myModelComponent.SetAnimationLooping(myAnimationStack.GetLast().myIsLooping);


	if (myAnimationStack.GetLast().myCoolDownTime != -1.f)
	{
		myAnimationStack.GetLast().myCoolDownTime -= aDeltaTime.GetSeconds();
		DL_PRINT("animation cooldown: %f", myAnimationStack.GetLast().myCoolDownTime);
		if (myAnimationStack.GetLast().myCoolDownTime <= 0.f)
		{
			if (myAnimationStack.GetLast().myNextAnimationKey == eAnimationState::invisible)
			{
				myModelComponent.SetVisibility(false);
			}

			myAnimationStack.Pop();

			if (myOnFinnishedCallback != nullptr)
			{
				myOnFinnishedCallback();
				myOnFinnishedCallback = nullptr;
			}
		}
	}
}

void CAnimationComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	if (myAnimationStates.empty() /*|| !myUpdateHasRun*/) return;
	auto animationEnd = myAnimationStates.end();
	auto it = animationEnd;

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
		}
		break;
	case eComponentMessageType::eDied:
		it = myAnimationStates.find("death");
		if (it != animationEnd)
		{
			PushAnimation(it->second);
		}
		break;
	case eComponentMessageType::eUnequip:
		if (myAnimationStack.GetLast().myAnimationKey == eAnimationState::unequip01 || myAnimationStack.GetLast().myAnimationKey == eAnimationState::equip01)
		{
			myOnFinnishedCallback = *aMessageData.myVoidFunction;
			break;
		}

		it = myAnimationStates.find("unequip");
		if (it != animationEnd)
		{
			myModelComponent.SetVisibility(true);
			PushAnimation(it->second);
			myOnFinnishedCallback = *aMessageData.myVoidFunction;
		}
		break;
	case eComponentMessageType::eEquip:
		it = myAnimationStates.find("equip");
		if (it != animationEnd)
		{
			if (myAnimationStack.GetLast().myAnimationKey == eAnimationState::equip01 || myAnimationStack.GetLast().myAnimationKey == eAnimationState::unequip01)
			{
				myAnimationStack.Pop();
			}
			PushAnimation(it->second);
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
