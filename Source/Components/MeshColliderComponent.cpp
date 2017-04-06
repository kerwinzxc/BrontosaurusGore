#include "stdafx.h"
#include "MeshColliderComponent.h"
#include "../Physics/Shape.h"
#include "../Physics/Physics.h"
#include "../Physics/PhysXManager.h"
#include "../Physics/Foundation.h"
#include "../Physics/PhysicsScene.h"

CMeshColliderComponent::CMeshColliderComponent(const SMeshColliderData& aColliderData, Physics::CPhysicsScene* aScene) : myScene(aScene),
	CColliderComponent(aColliderData, nullptr, nullptr)
{
	myData = aColliderData;
}


CMeshColliderComponent::~CMeshColliderComponent()
{
}

void CMeshColliderComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{

	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if (aMessageData.myComponent == this && myActor == nullptr)
		{
			const CU::Vector3f scale = GetParent()->GetToWorldTransform().GetScale();
			Init(scale);
			
		}
	}
	CColliderComponent::Receive(aMessageType, aMessageData);
}

void CMeshColliderComponent::Init(const CU::Vector3f& aScale)
{
	Physics::CPhysics* physics = Physics::CFoundation::GetInstance()->GetPhysics();
	Physics::SMaterialData material;
	Physics::CShape* shape = physics->CreateMeshShape(myData.myPath, material, aScale);
	
	if (!shape)
	{
		DL_ASSERT("Failed to create BoxShape");
	}

	shape->SetCollisionLayers(myData.myLayer, myData.myCollideAgainst);
	if(GetParent() != nullptr)
	{
		shape->SetObjectId(GetParent()->GetId());
	}

	Physics::CPhysicsActor* actor = physics->CreateStaticActor(shape, myData.IsTrigger);
	if (!actor)
	{
		DL_ASSERT("Failed to create physics actor");
	}
	myScene->AddActor(actor);
	myActor = actor;
	myShape = shape;
	myActor->SetCallbackData(this);
}
