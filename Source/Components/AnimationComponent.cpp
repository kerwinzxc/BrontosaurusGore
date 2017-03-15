#include "stdafx.h"
#include "AnimationComponent.h"


CAnimationComponent::CAnimationComponent(CModelComponent& aModelCompoent)
	: myModelComponent(aModelCompoent)
{
}

CAnimationComponent::~CAnimationComponent()
{
}

void CAnimationComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eMoving:
		//maybe check my velocity and update running/walking animation
	case eComponentMessageType::eShoot:
		//play shoot animation (if this is the real shoot message?)
		break;
	}
}
