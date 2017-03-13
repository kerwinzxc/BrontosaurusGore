#include "stdafx.h"
#include "SimulationEventCallback.h"
#include "PhysicsActor.h"
#include "PxSimulationEventCallback.h"
#include "PxRigidActor.h"

namespace Physics
{
	CSimulationEventCallback::CSimulationEventCallback()
	{
	}

	CSimulationEventCallback::~CSimulationEventCallback()
	{
	}

	void CSimulationEventCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
	{

	}

	void CSimulationEventCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
	{

	}

	void CSimulationEventCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
	{

	}

	void CSimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		for (physx::PxU32 i = 0; i < 1; i++)// change back to nbpairs if needed
		{
			const physx::PxContactPair& cp = pairs[i];

			IPhysicsCallback* actor = static_cast<IPhysicsCallback*>(pairHeader.actors[0]->userData);
			IPhysicsCallback* otherActor = static_cast<IPhysicsCallback*>(pairHeader.actors[1]->userData);

			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				if (actor != nullptr)
				{
					actor->OnCollisionEnter(otherActor);
				}
				if (otherActor != nullptr)
				{
					otherActor->OnCollisionEnter(actor);
				}
			}

			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				if (actor != nullptr)
				{
					actor->OnCollisionExit(otherActor);
				}
				if (otherActor != nullptr)
				{
					otherActor->OnCollisionExit(actor);
				}
			}
		}

		//	//Edvin är smet, Kevin är en best!
		//	//Edvin är semst, Kevin är best!
	}


	void CSimulationEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
	{
		for (physx::PxU32 i = 0; i < count; i++)// change back to count if needed
		{
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
				physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;

			IPhysicsCallback* triggerActor = static_cast<IPhysicsCallback*>(pairs[i].triggerActor->userData);
			IPhysicsCallback* otherActor = static_cast<IPhysicsCallback*>(pairs[i].otherActor->userData);

			if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				if (triggerActor != nullptr)
				{
					triggerActor->OnTriggerEnter(otherActor);
				}
				if (otherActor != nullptr)
				{
					otherActor->OnTriggerEnter(triggerActor);
				}
			}
			if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				if (triggerActor != nullptr)
				{
					triggerActor->OnTriggerExit(otherActor);
				}
				if (otherActor != nullptr)
				{
					otherActor->OnTriggerExit(triggerActor);
				}
			}
			if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
			{
				if (triggerActor != nullptr)
				{
					triggerActor->OnTriggerExit(otherActor);
				}
				if (otherActor != nullptr)
				{
					otherActor->OnTriggerExit(triggerActor);
				}
			}
		}







	}

	void CSimulationEventCallback::onAdvance(const physx::PxRigidBody*const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
	{

	}

}
