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
		void AddForce(const CU::Vector3f& aForce);
		void AddTorque(const CU::Vector3f& aTorque);

		void SetRotationLock(const CU::Vector3i& aRotationLocks);
		void SetShape(CShape* aShape) override;
		void SetMass(const float aMass);

		inline const EActorType GetType() const override { return EActorType::eDynamic; };
	private:
		physx::PxRigidDynamic* myDynamicActor;
		float myMass;
	};
}
