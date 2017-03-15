#include "stdafx.h"
#include "MeshColliderComponent.h"

CMeshColliderComponent::CMeshColliderComponent(const SMeshColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor)
	: CColliderComponent(aColliderData, aShape, aActor)
{
	myData = aColliderData;
}


CMeshColliderComponent::~CMeshColliderComponent()
{
}
