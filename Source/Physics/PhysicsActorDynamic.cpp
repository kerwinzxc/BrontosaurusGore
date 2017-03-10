#include "stdafx.h"
#include "PhysicsActorDynamic.h"
#include "PxRigidDynamic.h"
#include "extensions\PxRigidBodyExt.h"

namespace Physics
{
	CPhysicsActorDynamic::CPhysicsActorDynamic(physx::PxRigidDynamic* aActor, CShape* aShape) : CPhysicsActor(aActor, aShape)
	{
		myDynamicActor = aActor;
	}

	CPhysicsActorDynamic::~CPhysicsActorDynamic()
	{
		myDynamicActor = nullptr;
	}

	void CPhysicsActorDynamic::SetIsKinematic(const bool aIsKinematic)
	{
		myDynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, aIsKinematic);
	}

	void CPhysicsActorDynamic::SetRotationLock(const CU::Vector3i& aRotationLocks)
	{
		bool x, y, z = y = x = false;
		x = (bool)aRotationLocks.x;
		y = (bool)aRotationLocks.y;
		z = (bool)aRotationLocks.z;


		myDynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, x);
		myDynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, y);
		myDynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, z);
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