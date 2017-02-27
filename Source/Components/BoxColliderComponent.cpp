#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "../Physics/PhysXManager.h"

CBoxColliderComponent::CBoxColliderComponent(SBoxColliderData* aColliderData)
	: CColliderComponent(aColliderData)
{
	Physics::CPhysXManager& physxMan = Physics::CPhysXManager::GetInstance();

	myMaterial = physxMan.CreateMaterial(0.5f, 0.5f, 0.5f);
	myGeometry = physxMan.CreateBoxGeometry(aColliderData->mySize);
	myActor = physxMan.CreateStaticCollider(*myGeometry, *myMaterial);
	physxMan.AddActor(*myActor);
}

CBoxColliderComponent::~CBoxColliderComponent()
{

}
