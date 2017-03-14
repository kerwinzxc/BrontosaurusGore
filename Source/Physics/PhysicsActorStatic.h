#pragma once
#include "PhysicsActor.h"

namespace physx
{
	class PxRigidStatic;
}

namespace Physics
{
	class CPhysicsActorStatic : public CPhysicsActor
	{
	public:
		CPhysicsActorStatic(physx::PxRigidStatic* aActor, CShape* aShape);
		~CPhysicsActorStatic();

		inline const EActorType GetType() const override { return EActorType::eController; };

	private:
		physx::PxRigidStatic* myStaticActor;
	};
}
