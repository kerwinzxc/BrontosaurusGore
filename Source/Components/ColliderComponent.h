#pragma once
#include "Component.h"

namespace Physics
{
	class CPhysXManager;
}

namespace physx
{
	class PxGeometry;
	class PxMaterial;
	class PxRigidActor;
}


struct SColliderData 
{
	enum class eColliderType
	{
		eNone,
		eBox,
		eSphere,
		eCapsule,
		eMesh
	} myType = eColliderType::eNone;
	bool IsTrigger = false;
};

class CColliderComponent : public CComponent 
{
public:
	CColliderComponent(SColliderData* aColliderData);
	virtual ~CColliderComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

protected:
	physx::PxGeometry* myGeometry;
	physx::PxMaterial* myMaterial;
	physx::PxRigidActor* myActor;
};

