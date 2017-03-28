#include "stdafx.h"
#include "AnimationComponent.h"
#include "ModelComponent.h"

struct CAnimationComponent::SAnimationState
{
	std::string myAnimationKey;
	std::string myNextAnimationKey;
	float myAnimationBlend = 0.f;
	float myBlendTime = 0.f;
	float myLifeTime = INFINITY;
};

CAnimationComponent::CAnimationComponent(CModelComponent& aModelCompoent)
	: myAnimationStack(4u)
	, myModelComponent(aModelCompoent)
{
	myType = eComponentType::eAnimationComponent;

	SAnimationState idleState;
	idleState.myAnimationKey = "walk01";

	myAnimationStack.Add(idleState);
	myModelComponent.SetAnimation(myAnimationStack.GetLast().myAnimationKey);
	myModelComponent.SetNextAnimation("");
	myModelComponent.SetAnimationLerpValue(0.f);
}

CAnimationComponent::~CAnimationComponent()
{
}

void CAnimationComponent::Update(const CU::Time /*aDeltaTime*/)
{
}

void CAnimationComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eMoving:
	{
		CU::Vector2f lastPosition = myLastPosition;
		myLastPosition = CU::Vector2f(GetParent()->GetLocalTransform().myPosition.x, GetParent()->GetLocalTransform().myPosition.z);

		float speed = (myLastPosition - lastPosition).Length2();
		

		myModelComponent.SetAnimation("idle01");
		myModelComponent.SetNextAnimation("walk01");
		myModelComponent.SetAnimationLerpValue(/*speed*/0.4f/* += aDeltaTime.GetSeconds()*/);
		break;
	}
	case eComponentMessageType::eShoot:
		DL_PRINT("shot in animation component");
		myModelComponent.SetAnimation("shot");
		//play shoot animation (if this is the real shoot message?)
		break;
	}
}
