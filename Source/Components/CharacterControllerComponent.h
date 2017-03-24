#pragma once
#include "Component.h"
#include "..\Physics\IPhysicsCallback.h"

namespace Physics
{
	class CPhysicsCharacterController;
	class CPhysicsCallbackActor;
}

class CCharacterControllerComponent : public CComponent, public Physics::IPhysicsCallback
{
public:
	CCharacterControllerComponent(Physics::CPhysicsCharacterController* aController, const CU::Vector3f& aCenter = CU::Vector3f());
	~CCharacterControllerComponent();

	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	void OnTriggerEnter(Physics::CPhysicsCallbackActor* aOther) override;
	void OnTriggerExit(Physics::CPhysicsCallbackActor* aOther) override;
	void OnCollisionEnter(Physics::CPhysicsCallbackActor* aOther) override;
	void OnCollisionExit(Physics::CPhysicsCallbackActor* aOther) override;

private:
	Physics::CPhysicsCharacterController* myController;
	CU::Vector3f myCenter;
};

