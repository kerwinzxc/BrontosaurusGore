#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "../Physics/PhysXManager.h"


CBoxColliderComponent::CBoxColliderComponent(SBoxColliderData* aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor)
	: CColliderComponent(aColliderData, aShape, aActor)
{
	myData = *aColliderData;
}

CBoxColliderComponent::~CBoxColliderComponent()
{

}

