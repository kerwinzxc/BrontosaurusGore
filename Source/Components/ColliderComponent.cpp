#include "stdafx.h"
#include "ColliderComponent.h"
#include "ColliderComponentManager.h"
#include "../Physics/Shape.h"

CColliderComponent::CColliderComponent(const SColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor, const bool aIsTrigger)
{
	myData = aColliderData;
	myIsTrigger = aIsTrigger;
	myActor = aActor;
	if(myActor != nullptr)
	{
		myActor->SetCallbackData(this);
	}
	SetUserData(this);

	myShape = aShape;
	myType = eComponentType::eCollision;
	myManager = nullptr;
	myStoredCallBackDataList.Init(10);
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
	if(myActor != nullptr)
	{	
		CU::Matrix44f transform = myActor->GetTransformation();
		const CU::Matrix44f parentTransform = GetParent()->GetToWorldTransform();
		const CU::Vector3f scale = parentTransform.GetScale();
		transform.SetScale(scale);

		GetParent()->SetWorldTransformation(transform);
		SComponentMessageData data;
		data.myComponent = this;
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, data);
	}
}

void CColliderComponent::UpdateCallbacks()
{
	//kommentar: detta �r mycket snyggt! mvh carl
	for (SStoredCallBackData& callbackData : myStoredCallBackDataList)
	{
		SComponentMessageData data;
		data.myComponent = static_cast<CComponent*>(callbackData.componentPtr);
		GetParent()->NotifyComponents(callbackData.collisionType, data);
	}
	myStoredCallBackDataList.RemoveAll();
}

void CColliderComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent: 
		if (aMessageData.myComponentTypeAdded == eComponentType::eCollision && aMessageData.myComponent == this)
		{
			if (myShape)
			{
				const int ParentId = GetParent()->GetId();
				myShape->SetObjectId(ParentId);
			}
			CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
			transformation.SetScale({ 1.0f, 1.0f, 1.0f });
			CU::Vector3f worldPos = myData.center;
			transformation.SetPosition(transformation.GetPosition() + worldPos);
			myActor->SetTransformation(transformation);
		}	
		
	case eComponentMessageType::eObjectDone:
	case eComponentMessageType::eMoving:
	{
		if (aMessageData.myComponent != this && myActor != nullptr)
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
	case eComponentMessageType::eActivate:
		myActor->SetIsActive(true);
		break;
	case eComponentMessageType::eDeactivate:
		myActor->SetIsActive(false);
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
	else if (aQuestionType == eComponentQuestionType::eHaveTriggerCollision)
	{
		return myIsTrigger;
	}
	return false;
}

void CColliderComponent::OnTriggerEnter(Physics::CPhysicsCallbackActor* aOther)
{
	if (aOther->GetCallbackData())
	{
		myStoredCallBackDataList.Add({ aOther->GetCallbackData()->GetUserData(), eComponentMessageType::eOnTriggerEnter });
	}
}

void CColliderComponent::OnTriggerExit(Physics::CPhysicsCallbackActor* aOther)
{
	if (aOther->GetCallbackData())
	{
		myStoredCallBackDataList.Add({ aOther->GetCallbackData()->GetUserData(), eComponentMessageType::eOnTriggerExit });
	}
}

void CColliderComponent::OnCollisionEnter(Physics::CPhysicsCallbackActor* aOther)
{
	if (aOther->GetCallbackData())
	{
		myStoredCallBackDataList.Add({ aOther->GetCallbackData()->GetUserData(), eComponentMessageType::eOnCollisionEnter });
	}
}

void CColliderComponent::OnCollisionExit(Physics::CPhysicsCallbackActor* aOther)
{
	if (aOther->GetCallbackData())
	{
		myStoredCallBackDataList.Add({ aOther->GetCallbackData()->GetUserData(), eComponentMessageType::eOnCollisionExit });
	}
}
