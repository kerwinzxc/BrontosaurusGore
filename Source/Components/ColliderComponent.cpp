#include "stdafx.h"
#include "ColliderComponent.h"
#include "../Physics/PhysXManager.h"
#include <PxRigidActor.h>
#include <PxMaterial.h>

CColliderComponent::CColliderComponent(SColliderData * aColliderData)
{
	myGeometry = nullptr;
	myMaterial = nullptr;
	myActor = nullptr;	
	myType = eComponentType::eCollision;
}

CColliderComponent::~CColliderComponent()
{
	if (myActor != nullptr)
	{
		Physics::CPhysXManager::GetInstance().RemoveActor(*myActor);
		myActor->release();
		myActor = nullptr;
	}
	if (myGeometry != nullptr)
	{
		delete myGeometry;
		myGeometry = nullptr;
	}
	if (myMaterial != nullptr)
	{
		myMaterial->release();
		myMaterial = nullptr;
	}
}

void CColliderComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent: 
		if (aMessageData.myComponentTypeAdded != eComponentType::eCollision) break; //else: fall through;
	case eComponentMessageType::eObjectDone:
	case eComponentMessageType::eMoving:
		Physics::CPhysXManager::GetInstance().SetGlobalPose(myActor, GetParent()->GetToWorldTransform());
		break;
	case eComponentMessageType::eOnCollisionEnter:
		break;
	case eComponentMessageType::eOnCollisionExit:
		break;
	case eComponentMessageType::eSetIsColliderActive:
		break;
	default:
		break;
	}
}
