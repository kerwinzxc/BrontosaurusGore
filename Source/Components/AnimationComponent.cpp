#include "stdafx.h"
#include "AnimationComponent.h"
#include "ModelComponent.h"

#include "../TShared/AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

CU::GrowingArray<CAnimationComponent*> CAnimationComponent::ourAnimations(64u);

struct CAnimationComponent::SAnimation
{
	eAnimationState myAnimationKey;
	eAnimationState myNextAnimationKey;
	float myAnimationBlend = 0.f;
	float myBlendTime = 0.f;
	float myLifeTime = INFINITY;
};

CAnimationComponent::CAnimationComponent(CModelComponent& aModelCompoent)
	: myAnimationStack(4u)
	, myModelComponent(aModelCompoent)
	, myMainTimer(0.f)
{
	myType = eComponentType::eAnimationComponent;

	SAnimation idleState;
	idleState.myAnimationKey = eAnimationState::walk01;

	myAnimationStack.Add(idleState);
	myModelComponent.SetAnimation(myAnimationStack.GetLast().myAnimationKey);
	myModelComponent.SetNextAnimation(eAnimationState::none);
	myModelComponent.SetAnimationLerpValue(0.f);
}

CAnimationComponent::~CAnimationComponent()
{
}

void CAnimationComponent::Update(const CU::Time aDeltaTime)
{
	myMainTimer += aDeltaTime.GetSeconds();

	if (myMainTimer < 0.5f)
	{
		myModelComponent.SetAnimation(eAnimationState::idle01);
		myModelComponent.SetNextAnimation(eAnimationState::walk01);
		myModelComponent.SetAnimationLerpValue(myMainTimer / 0.5f);
	}
	else if (myMainTimer < 1.f)
	{
		myModelComponent.SetAnimation(eAnimationState::walk01);
		myModelComponent.SetNextAnimation(eAnimationState::run01);
		myModelComponent.SetAnimationLerpValue((myMainTimer - 0.5f) / 0.5f);
	}
	else if (myMainTimer < 1.5f)
	{
		myModelComponent.SetAnimation(eAnimationState::run01);
		myModelComponent.SetNextAnimation(eAnimationState::walk01);
		myModelComponent.SetAnimationLerpValue((myMainTimer - 1.f) / 0.5f);
	}
	else if (myMainTimer < 2.f)
	{
		myModelComponent.SetAnimation(eAnimationState::walk01);
		myModelComponent.SetNextAnimation(eAnimationState::idle01);
		myModelComponent.SetAnimationLerpValue((myMainTimer - 1.5f) / 0.5f);
	}
	else
	{
		myMainTimer = 0.f;
	}
}

void CAnimationComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	aMessageType;
	aMessageData;

	//switch (aMessageType)
	//{
	//case eComponentMessageType::eMoving:
	//{
	//	CU::Vector2f lastPosition = myLastPosition;
	//	myLastPosition = CU::Vector2f(GetParent()->GetLocalTransform().myPosition.x, GetParent()->GetLocalTransform().myPosition.z);
	//	
	//	myModelComponent.SetAnimation(/*"idle01"*/eAnimationState::idle01);
	//	//myModelComponent.SetNextAnimation("walk01");
	//	myModelComponent.SetAnimationLerpValue(/*speed*/0.4f/* += aDeltaTime.GetSeconds()*/);
	//	break;
	//}
	//case eComponentMessageType::eShoot:
	//	DL_PRINT("shot in animation component");
	//	myModelComponent.SetAnimation(eAnimationState::shot01);
	//	//play shoot animation (if this is the real shoot message?)
	//	break;
	//}
}

void CAnimationComponent::UpdateAnimations(const CU::Time aDeltaTime)
{
	for (CAnimationComponent* animationComponent : ourAnimations)
	{
		animationComponent->Update(aDeltaTime);
	}
}
