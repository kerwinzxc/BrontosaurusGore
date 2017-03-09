#pragma once
#include "ColliderComponent.h"
#include "BoxColliderComponent.h"

struct SRigidBodyData : public SColliderData
{
	SRigidBodyData() { myType = eColliderType::eRigidbody; }
	float mass;
	float angularDrag;
	float useGravity;
	float isKinematic;

	CU::Vector3i freezedRotationAxiees;

	//union
	//{
	//	SBoxColliderData boxData;
	//};
};


class CRigidBodyComponent : public CColliderComponent
{
public:
	CRigidBodyComponent(SRigidBodyData* aColliderData, Physics::CPhysicsActor* aActor);
	~CRigidBodyComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	inline SColliderData::eColliderType GetType() { return SColliderData::eColliderType::eRigidbody; };

private:
	SRigidBodyData myData;
};

