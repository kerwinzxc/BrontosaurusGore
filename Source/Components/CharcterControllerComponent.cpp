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

void CCharcterControllerComponent::OnTriggerEnter(const IPhysicsCallback* aOther)
{
	DL_PRINT("OTHER ENTERED ME: %d", (int)aOther);
}

void CCharcterControllerComponent::OnTriggerExit(const IPhysicsCallback* aOther)
{
	DL_PRINT("OTHER EXITED ME: %d", (int)aOther);
}

void CCharcterControllerComponent::OnCollisionEnter(const IPhysicsCallback* aOther)
{
	DL_PRINT("OTHER COLLIDES WITH ME: %d", (int)aOther);
}

void CCharcterControllerComponent::OnCollisionExit(const IPhysicsCallback* aOther)
{
	DL_PRINT("OTHER UN-COLLIDES WITH ME: %d", (int)aOther);
}

