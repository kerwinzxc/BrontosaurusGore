#include "stdafx.h"
#include "CharcterControllerComponent.h"
#include "..\Physics\PhysicsCharacterController.h"


CCharcterControllerComponent::CCharcterControllerComponent(Physics::CPhysicsCharacterController * aController)
{
	myController = aController;
	myType = eComponentType::eCharacterController;
	myController->SetCallbackData(this);
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

void CCharcterControllerComponent::OnTriggerEnter(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyOnlyComponents(eComponentMessageType::eOnTriggerEnter, data);
	DL_PRINT("TriggerEnter");

}

void CCharcterControllerComponent::OnTriggerExit(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyOnlyComponents(eComponentMessageType::eOnTriggerExit, data);
	DL_PRINT("TriggerExit");
}

void CCharcterControllerComponent::OnCollisionEnter(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyOnlyComponents(eComponentMessageType::eOnCollisionEnter, data);
	DL_PRINT("ColEnter");
}

void CCharcterControllerComponent::OnCollisionExit(Physics::CPhysicsCallbackActor* aOther)
{
	void* compPtr = aOther->GetCallbackData()->GetUserData();
	SComponentMessageData data;
	data.myComponent = static_cast<CComponent*>(compPtr);
	GetParent()->NotifyOnlyComponents(eComponentMessageType::eOnCollisionExit, data);
	DL_PRINT("ColExit");

}

