#include "stdafx.h"
#include "AnimationComponent.h"
#include "ModelComponent.h"
#include "AnimationComponentLoader.h"

#include "../CommonUtilities/JsonValue.h"
#include "../TShared/AnimationState.h"
#include "../Game/PollingStation.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

#define NU_HAR_JAG_DRAMATISERAT_KLART_MIN_D�D 3.14f

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
	if (myAnimationStack.Empty())
	{
		return;
	}

	SAnimation* currentAnimation = &myAnimationStack.GetLast();

	if (currentAnimation->myBlendTime > 0.f)
	{
		if (currentAnimation->myBlendTime < currentAnimation->myAnimationBlend)
		{
			currentAnimation->myAnimationBlend += aDeltaTime.GetSeconds();
		}
	}

	CU::Vector3f position3D = GetParent()->GetToWorldTransform().GetPosition();
	CU::Vector2f position(position3D.x, position3D.z);
	CU::Vector2f velocity = position - myLastPosition;
	myLastPosition = position;

	float speed = velocity.Length();

	if (speed > 0.01f && currentAnimation->myAnimationKey == eAnimationState::idle01)
	{
		auto& it = myAnimationStates.find("moving");
		if (it != myAnimationStates.end())
		{
			currentAnimation = &it->second;
			DL_PRINT("has moving");
			currentAnimation->myAnimationBlend = 0.f;
		}
	}
	
	myModelComponent.SetAnimation(currentAnimation->myAnimationKey);
	myModelComponent.SetNextAnimation(currentAnimation->myNextAnimationKey);
	myModelComponent.SetAnimationLerpValue(currentAnimation->myAnimationBlend);
	myModelComponent.SetAnimationLooping(currentAnimation->myIsLooping);

	if (currentAnimation->myCoolDownTime != -1.f)
	{
		currentAnimation->myCoolDownTime -= aDeltaTime.GetSeconds();
		DL_PRINT("animation cooldown: %f", currentAnimation->myCoolDownTime);
		if (currentAnimation->myCoolDownTime <= 0.f)
		{
			if (currentAnimation->myNextAnimationKey == eAnimationState::invisible)
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
