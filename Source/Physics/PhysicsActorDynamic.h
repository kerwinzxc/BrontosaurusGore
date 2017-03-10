#pragma once
#include "PhysicsActor.h"

namespace physx 
{
	class PxRigidDynamic;
}

namespace Physics
{
	class CPhysicsActorDynamic : public CPhysicsActor
	{
	public:
		CPhysicsActorDynamic(physx::PxRigidDynamic* aActor, CShape* aShape);
		~CPhysicsActorDynamic();

		void SetIsKinematic(const bool aIsKinematic);
		inline const EActorType GetType() const override { return EActorType::eDynamic; }

		void SetRotationLock(const CU::Vector3i& aRotationLocks);

		void SetShape(CShape* aShape) override;
		void SetMass(const float aMass);
	private:
		physx::PxRigidDynamic* myDynamicActor;
		float myMass;
	};
}
