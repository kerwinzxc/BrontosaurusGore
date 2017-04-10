#pragma once
#include "ColliderComponent.h"
#include "BoxColliderComponent.h"

struct SRigidBodyData : public SColliderData
{
	SRigidBodyData() { myType = eColliderType::eRigidbody; }
	float mass = 1.0f;
	float angularDrag = 1.0f;
	float useGravity = true;
	float isKinematic = false;

	CU::Vector3<bool> freezedRotationAxiees;
};


class CRigidBodyComponent : public CColliderComponent
{
public:
	CRigidBodyComponent(const SRigidBodyData& aColliderData, Physics::CPhysicsActor* aActor, const bool aIsTrigger);
	~CRigidBodyComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	inline SColliderData::eColliderType GetType() { return SColliderData::eColliderType::eRigidbody; };

private:
	SRigidBodyData myData;
};

