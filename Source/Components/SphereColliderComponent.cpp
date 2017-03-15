#include "stdafx.h"
#include "SphereColliderComponent.h"


CSphereColliderComponent::CSphereColliderComponent(const SSphereColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor)
	: CColliderComponent(aColliderData, aShape, aActor)
{
	myData = aColliderData;
}

CSphereColliderComponent::~CSphereColliderComponent()
{
}
