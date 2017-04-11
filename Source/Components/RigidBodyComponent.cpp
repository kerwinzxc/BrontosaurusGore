#include "stdafx.h"
#include "RigidBodyComponent.h"
#include "..\Physics\Shape.h"
#include "..\Physics\PhysicsActorDynamic.h"
#include "ColliderComponentManager.h"


CRigidBodyComponent::CRigidBodyComponent(const SRigidBodyData& aColliderData, Physics::CPhysicsActor* aActor, const bool aIsTrigger)
	: CColliderComponent(aColliderData, nullptr, aActor, aIsTrigger)
{
	myData = aColliderData;
}

CRigidBodyComponent::~CRigidBodyComponent()
{
}

void CRigidBodyComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if (aMessageData.myComponentTypeAdded != eComponentType::eCollision) break; //else: fall through;
	case eComponentMessageType::eObjectDone:
	{
		SComponentQuestionData data;
		if (GetParent()->AskComponents(eComponentQuestionType::eGetCollisionShape, data))
		{
			myShape = new Physics::CShape(*data.myCollider->GetShape());
			reinterpret_cast<Physics::CPhysicsActorDynamic*>(myActor)->SetMass(myData.mass);
			myActor->SetShape(myShape);
			myData.center = data.myCollider->GetData()->center;
			GetManager()->RemoveActorFromScene(data.myCollider->GetActor());
			GetParent()->RemoveComponent(data.myCollider);
		}
		break;
	}
	case eComponentMessageType::eAddForce:
	{
		static_cast<Physics::CPhysicsActorDynamic*>(myActor)->AddForce(aMessageData.myVector3f);
		break;
	}
	}

	CColliderComponent::Receive(aMessageType, aMessageData);
}
