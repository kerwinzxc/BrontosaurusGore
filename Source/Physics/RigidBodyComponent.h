#pragma once
#include "Components/Component.h"

class CRigidBodyComponent :
	public CComponent
{
public:
	friend class CPhysicsManager;

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	void Destroy()override;
private:

	CRigidBodyComponent();
	~CRigidBodyComponent();
	ComponentId myRigidBodyId = NULL_COMPONENT;
	CPhysicsManager* myManager;
};
