#include "stdafx.h"
#include "CapsuleColliderComponent.h"

CCapsuleColliderComponent::CCapsuleColliderComponent(const SCapsuleColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor, const bool aIsTrigger)
	: CColliderComponent(aColliderData, aShape, aActor , aIsTrigger)
{
	myData = aColliderData;
}

CCapsuleColliderComponent::~CCapsuleColliderComponent()
{
}
