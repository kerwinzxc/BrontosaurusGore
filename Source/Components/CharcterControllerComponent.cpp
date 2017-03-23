#include "stdafx.h"
#include "CharcterControllerComponent.h"
#include "..\Physics\PhysicsCharacterController.h"


CCharcterControllerComponent::CCharcterControllerComponent(Physics::CPhysicsCharacterController* aController, const CU::Vector3f& aCenter)
{
	myCenter = aCenter;
	myController = aController;
	myController->SetCallbackData(this);
	myType = eComponentType::eCharacterController;
	SetUserData(this);
}

CCharcterControllerComponent::~CCharcterControllerComponent()
{
}

bool CCharcterControllerComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eMovePhysicsController:
	{
		myController->Move(aQuestionData.myVector3f, aQuestionData.myVector4f.w);
		aQuestionData.myVector3f = myController->GetPosition();
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

void CCharcterControllerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if (aMessageData.myComponentTypeAdded != eComponentType::eCharacterController) break; //else: fall through;
	case eComponentMessageType::eObjectDone:
	{
		CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
		//transformation.SetScale({ 1.0f, 1.0f, 1.0f });
		//CU::Vector3f worldPos = myData.center;
		//transformation.SetPosition(transformation.GetPosition() + worldPos);
		myController->SetPosition(transformation.myPosition);
	break;
	}
	case eComponentMessageType::eSetControllerPosition:
	{
		//transformation.SetScale({ 1.0f, 1.0f, 1.0f });
		//CU::Vector3f worldPos = myData.center;
		//transformation.SetPosition(transformation.GetPosition() + worldPos);
		myController->SetPosition(aMessageData.myVector3f);
		break;
	}
	default:
		break;
	}

}

void CCharcterControllerComponent::OnTriggerEnter(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyComponents(eComponentMessageType::eOnTriggerEnter, data);
	DL_PRINT("TriggerEnter");

}

void CCharcterControllerComponent::OnTriggerExit(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyComponents(eComponentMessageType::eOnTriggerExit, data);
	DL_PRINT("TriggerExit");
}

void CCharcterControllerComponent::OnCollisionEnter(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyComponents(eComponentMessageType::eOnCollisionEnter, data);
	DL_PRINT("ColEnter");
}

void CCharcterControllerComponent::OnCollisionExit(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyComponents(eComponentMessageType::eOnCollisionExit, data);
	DL_PRINT("ColExit");

}

