#pragma once
#include <PxSimulationEventCallback.h>
#include <characterkinematic/PxController.h>

namespace physx
{
	struct PxConstraintInfo;
	class PxActor;
}

namespace Physics
{
	class CSimulationEventCallback : public physx::PxSimulationEventCallback , public physx::PxUserControllerHitReport
	{
	public:
		CSimulationEventCallback();
		~CSimulationEventCallback();

		void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count);
		void onWake(physx::PxActor** actors, physx::PxU32 count);
		void onSleep(physx::PxActor** actors, physx::PxU32 count);
		void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
		void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
		void onAdvance(const physx::PxRigidBody*const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count);

		virtual void onShapeHit(const physx::PxControllerShapeHit& hit) override;


		virtual void onControllerHit(const physx::PxControllersHit& hit) override;


		virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;

	};
}
