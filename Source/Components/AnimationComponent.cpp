#include "stdafx.h"
#include "AnimationComponent.h"
#include "ModelComponent.h"

#include "../CommonUtilities/JsonValue.h"
#include "../TShared/AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

CU::GrowingArray<CAnimationComponent*> CAnimationComponent::ourAnimations(64u);

struct CAnimationComponent::SAnimation
{
	eAnimationState myAnimationKey;
	eAnimationState myNextAnimationKey;
	float myAnimationBlend = 0.f;
	float myBlendTime = 0.f;
	float myLifeTime = FLT_MAX;
	bool myIsLooping = true;
};

static CAnimationComponent::SAnimation idleState;
static CAnimationComponent::SAnimation shotState;

CAnimationComponent::CAnimationComponent(CModelComponent& aModelCompoent)
	: myAnimationStack(4u)
	, myModelComponent(aModelCompoent)
	, myMainTimer(0.f)
{

	//CU::CJsonValue animationStates("Models/Animations/AnimationStates.json");

	myType = eComponentType::eAnimationComponent;

	idleState.myAnimationKey = eAnimationState::shot01;
	idleState.myNextAnimationKey = eAnimationState::none;

	shotState.myAnimationKey = eAnimationState::shot01;
	shotState.myNextAnimationKey = eAnimationState::none;
	shotState.myIsLooping = false;
	shotState.myLifeTime = 2.f;

	myAnimationStack.Add(idleState);
	myModelComponent.SetAnimation(myAnimationStack.GetLast().myAnimationKey);
	myModelComponent.SetNextAnimation(eAnimationState::none);
	myModelComponent.SetAnimationLerpValue(0.f);

	ourAnimations.Add(this);
}

CAnimationComponent::~CAnimationComponent()
{
	ourAnimations.RemoveCyclic(this);
}

void CAnimationComponent::Update(const CU::Time aDeltaTime)
{
	myMainTimer += aDeltaTime.GetSeconds();

	//if (myAnimationStack.GetLast().myBlendTime > 0.f)
	//{
	//	if (myAnimationStack.GetLast().myBlendTime < myAnimationStack.GetLast().myAnimationBlend)
	//	{
	//		myAnimationStack.GetLast().myAnimationBlend += aDeltaTime.GetSeconds();
	//	}
	//}


	//myModelComponent.SetAnimation(myAnimationStack.GetLast().myAnimationKey);
	//myModelComponent.SetNextAnimation(myAnimationStack.GetLast().myNextAnimationKey);
	//myModelComponent.SetAnimationLerpValue(myAnimationStack.GetLast().myBlendTime);
	//myModelComponent.SetAnimationLooping(myAnimationStack.GetLast().myIsLooping);


	//if (myMainTimer >= myAnimationStack.GetLast().myLifeTime)
	//{
	//	if (myAnimationStack.GetLast().myNextAnimationKey == eAnimationState::none)
	//	{
	//		myAnimationStack.Pop();
	//	}
	//	else
	//	{
	//		switch (myAnimationStack.GetLast().myNextAnimationKey)
	//		{
	//		case eAnimationState::idle01:
	//			//break;
	//		case eAnimationState::walk01:
	//			//break;
	//		case eAnimationState::run01:
	//			//break;
	//		case eAnimationState::shot01:
	//			//break;
	//		default:
	//			DL_PRINT_WARNING("Error in animation, trying to change to animation %s but it is not available", SAnimationState::AnimationStates[(int)myAnimationStack.GetLast().myNextAnimationKey]);
	//			myAnimationStack.Pop();
	//			break;
	//		}
	//	}
	//}

	if (myMainTimer < 4.f)
	{
		myModelComponent.SetAnimation(eAnimationState::idle01);
		myModelComponent.SetNextAnimation(eAnimationState::walk01);
		myModelComponent.SetAnimationLerpValue(myMainTimer / 4.f);
	}
	else if (myMainTimer < 8.f)
	{
		myModelComponent.SetAnimation(eAnimationState::walk01);
		myModelComponent.SetNextAnimation(eAnimationState::idle01);
		myModelComponent.SetAnimationLerpValue((myMainTimer - 4.f) / 4.f);
	}
	else
	{
		myMainTimer = 0.f;
	}

	myModelComponent.SetAnimation(eAnimationState::shot01);
	myModelComponent.SetNextAnimation(eAnimationState::none);
}

void CAnimationComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	aMessageType;
	aMessageData;

	switch (aMessageType)
	{
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
	case eComponentMessageType::eShoot:
		DL_PRINT("shot in animation component");
		//myModelComponent.SetAnimation(eAnimationState::shot01);
		//if (myAnimationStack.GetLast().myAnimationKey != eAnimationState::shot01)
		//{
		//	myAnimationStack.Add(shotState);
		//	myModelComponent.ResetAnimation();
		//	myMainTimer = 0.f;
		//}
		//play shoot animation (if this is the real shoot message?)
		break;
	}
}

void CAnimationComponent::UpdateAnimations(const CU::Time aDeltaTime)
{
	for (CAnimationComponent* animationComponent : ourAnimations)
	{
		animationComponent->Update(aDeltaTime);
	}
}
