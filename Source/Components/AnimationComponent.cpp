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
	myAnimationStack.Add("idle");
	myModelComponent.SetAnimation(myAnimationStack.GetLast());
	myModelComponent.SetNextAnimation("");
	myModelComponent.SetAnimationLerpValue(0.f);
}

CAnimationComponent::~CAnimationComponent()
{
}

void CAnimationComponent::Update(const CU::Time aDeltaTime)
{

}

void CAnimationComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eMoving:
		//maybe check my velocity and update running/walking animation
		break;
	case eComponentMessageType::eShoot:
		//play shoot animation (if this is the real shoot message?)
		break;
	}
}
