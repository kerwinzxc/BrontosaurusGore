#include "stdafx.h"
#include "SphereColliderComponent.h"


CSphereColliderComponent::CSphereColliderComponent(const SSphereColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor, const bool aIsTrigger)
	: CColliderComponent(aColliderData, aShape, aActor, aIsTrigger)
{
	myData = aColliderData;
}

CSphereColliderComponent::~CSphereColliderComponent()
{
}
