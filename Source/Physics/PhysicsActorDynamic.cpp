#include "stdafx.h"
#include "PhysicsActorDynamic.h"
#include "PxRigidDynamic.h"
#include "extensions\PxRigidBodyExt.h"

namespace Physics
{
	CPhysicsActorDynamic::CPhysicsActorDynamic(physx::PxRigidDynamic* aActor, CShape* aShape) : CPhysicsActor(aActor, aShape)
	{
		myDynamicActor = aActor;
		myIsKinematic = false;
	}

	CPhysicsActorDynamic::~CPhysicsActorDynamic()
	{
		myDynamicActor = nullptr;
	}

	void CPhysicsActorDynamic::SetIsKinematic(const bool aIsKinematic)
	{
		myIsKinematic = aIsKinematic;
		myDynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, aIsKinematic);
	}

	void CPhysicsActorDynamic::AddForce(const CU::Vector3f& aForce)
	{
		if(!myIsKinematic)
		{
			myDynamicActor->addForce({ aForce.x, aForce.y, aForce.z });
		}
	}

	void CPhysicsActorDynamic::AddTorque(const CU::Vector3f& aTorque)
	{
		if (!myIsKinematic)
			myDynamicActor->addTorque({ aTorque.x, aTorque.y, aTorque.z });
	}

	void CPhysicsActorDynamic::SetRotationLock(const CU::Vector3<bool>& aRotationLocks)
	{
		myDynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, aRotationLocks.x);
		myDynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, aRotationLocks.y);
		myDynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, aRotationLocks.z);
	}

	void CPhysicsActorDynamic::SetShape(CShape* aShape)
	{
		CPhysicsActor::SetShape(aShape);
		physx::PxRigidBodyExt::updateMassAndInertia(*myDynamicActor, myMass);
	}

	void CPhysicsActorDynamic::SetMass(const float aMass)
	{
		myMass = aMass;
		if(myShape)
			physx::PxRigidBodyExt::updateMassAndInertia(*myDynamicActor, aMass);
	}
}