#include "stdafx.h"
#include "CharcterControllerComponent.h"
#include "..\Physics\PhysicsCharacterController.h"


CCharcterControllerComponent::CCharcterControllerComponent(Physics::CPhysicsCharacterController * aController)
{
	myController = aController;
	myType = eComponentType::eCharacterController;
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
	case eComponentQuestionType::ePhysicsControllerGrounded:
	{
		aQuestionData.myBool = myController->GetIsGrounded();
		return true;
		break;
	}
	default:
		break;
	}
	return false;
}
