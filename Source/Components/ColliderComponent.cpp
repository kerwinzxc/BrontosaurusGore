#include "stdafx.h"
#include "ColliderComponent.h"
#include "ColliderComponentManager.h"

CColliderComponent::CColliderComponent(SColliderData* aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor)
{
	myData = *aColliderData;

	myActor = aActor;
	myShape = aShape;
	myType = eComponentType::eCollision;
	myManager = nullptr;
}

CColliderComponent::~CColliderComponent()
{
	// Remove me from scene baby
	//myManager->RemoveActorFromScene(myActor);
	myManager = nullptr;


	if (myActor != nullptr)
		delete myActor;
	myActor = nullptr;

	/*if (myShape)
		delete myShape;
	myShape = nullptr;*/
}

void CColliderComponent::UpdatePosition()
{
	GetParent()->SetWorldTransformation(myActor->GetTransformation());
	SComponentMessageData data;
	data.myBool = false;
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, data);
}

void CColliderComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent: 
		if (aMessageData.myComponentTypeAdded != eComponentType::eCollision) break; //else: fall through;
	case eComponentMessageType::eObjectDone:
	case eComponentMessageType::eMoving:
	{
		if (aMessageData.myBool == false)
		{

			CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
			transformation.SetScale({ 1.0f, 1.0f, 1.0f });
			CU::Vector3f worldPos = myData.center;
			transformation.SetPosition(transformation.GetPosition() + worldPos);
			myActor->SetTransformation(transformation);
		}
		break;
	}
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

bool CColliderComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	if (aQuestionType == eComponentQuestionType::eGetCollisionShape)
	{
		if (myData.myType != SColliderData::eColliderType::eRigidbody)
		{
			if (myShape != nullptr)
			{
				aQuestionData.myCollider = this;
				return true;
			}
		}
	}
	return false;
}
