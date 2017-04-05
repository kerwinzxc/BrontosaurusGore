#include "stdafx.h"
#include "CharacterControllerComponent.h"
#include "..\Physics\PhysicsCharacterController.h"


CCharacterControllerComponent::CCharacterControllerComponent(Physics::CPhysicsCharacterController* aController, const float aHalfHeight, const float aRadius, const CU::Vector3f& aCenter)
{
	myCenter = aCenter;
	myRadius = aRadius;
	myCenter.y += ((aHalfHeight * 2) - (aRadius / 2));
	myController = aController;
	myController->SetCallbackData(this);
	myType = eComponentType::eCharacterController;
	SetUserData(this);
}

CCharacterControllerComponent::~CCharacterControllerComponent()
{
}

bool CCharacterControllerComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eMovePhysicsController:
	{
		myController->Move(aQuestionData.myVector3f, aQuestionData.myVector4f.w);
		aQuestionData.myVector3f = myController->GetPosition() - myCenter;
		return true;
		break;
	}
	case eComponentQuestionType::ePhysicsControllerConstraints:
	{
		aQuestionData.myChar = myController->GetConstraints();
		return true;
		break;
	}
	default:
		break;
	}
	return false;
}

void CCharacterControllerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if (aMessageData.myComponentTypeAdded != eComponentType::eCharacterController) break; //else: fall through;
	case eComponentMessageType::eObjectDone:
	{
		CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
		transformation.SetScale({ 1.0f, 1.0f, 1.0f });
		transformation.SetPosition(transformation.GetPosition() /*- myCenter*/);
		myController->SetPosition(transformation.myPosition);
		break;
	}
	case eComponentMessageType::eActivate:
	{
		myController->SetIsActive(true);
		break;
	}
	case eComponentMessageType::eDeactivate:
	{
		myController->SetIsActive(false);
		break;
	}
	case eComponentMessageType::eSetControllerPosition:
	{
		//transformation.SetScale({ 1.0f, 1.0f, 1.0f });
		//transformation.SetPosition(transformation.GetPosition() + myCenter);
		myController->SetPosition(aMessageData.myVector3f - myCenter);
		break;
	}

	default:
		break;
	}

}

void CCharacterControllerComponent::OnTriggerEnter(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyComponents(eComponentMessageType::eOnTriggerEnter, data);
	DL_PRINT("TriggerEnter");
}

void CCharacterControllerComponent::OnTriggerExit(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyComponents(eComponentMessageType::eOnTriggerExit, data);
	DL_PRINT("TriggerExit");
}

void CCharacterControllerComponent::OnCollisionEnter(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyComponents(eComponentMessageType::eOnCollisionEnter, data);
	DL_PRINT("ColEnter");
}

void CCharacterControllerComponent::OnCollisionExit(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyComponents(eComponentMessageType::eOnCollisionExit, data);
	DL_PRINT("ColExit");
}

