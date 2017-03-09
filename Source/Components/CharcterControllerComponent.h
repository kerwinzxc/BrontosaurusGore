#pragma once
#include "Component.h"

namespace Physics
{
	class CPhysicsCharacterController;
}

class CCharcterControllerComponent : public CComponent
{
public:
	CCharcterControllerComponent(Physics::CPhysicsCharacterController* aController);
	~CCharcterControllerComponent();

	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;

private:
	Physics::CPhysicsCharacterController* myController;
};

