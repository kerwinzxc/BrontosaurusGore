#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "../Physics/PhysXManager.h"


CBoxColliderComponent::CBoxColliderComponent(const SBoxColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor)
	: CColliderComponent(aColliderData, aShape, aActor)
{
}

CBoxColliderComponent::~CBoxColliderComponent()
{

}

