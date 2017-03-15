#include "stdafx.h"
#include "CapsuleColliderComponent.h"

CCapsuleColliderComponent::CCapsuleColliderComponent(const SCapsuleColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor)
	: CColliderComponent(aColliderData, aShape, aActor)
{
	myData = aColliderData;
}

CCapsuleColliderComponent::~CCapsuleColliderComponent()
{
}
