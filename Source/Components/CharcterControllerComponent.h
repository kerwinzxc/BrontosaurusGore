#pragma once
#include "Component.h"
#include "..\Physics\IPhysicsCallback.h"

namespace Physics
{
	class CPhysicsCharacterController;
}

class CCharcterControllerComponent : public CComponent, public Physics::IPhysicsCallback
{
public:
	CCharcterControllerComponent(Physics::CPhysicsCharacterController* aController);
	~CCharcterControllerComponent();

	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;

	void OnTriggerEnter(const IPhysicsCallback* aOther) override;
	void OnTriggerExit(const IPhysicsCallback* aOther) override;
	void OnCollisionEnter(const IPhysicsCallback* aOther) override;
	void OnCollisionExit(const IPhysicsCallback* aOther) override;

private:
	Physics::CPhysicsCharacterController* myController;
};

