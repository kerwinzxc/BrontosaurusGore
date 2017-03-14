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

			CPhysicsCallbackActor* actor = static_cast<CPhysicsCallbackActor*>(pairHeader.actors[0]->userData);
			IPhysicsCallback* actorCallback = static_cast<IPhysicsCallback*>(actor->GetCallbackData());

			CPhysicsCallbackActor* otherActor = static_cast<CPhysicsCallbackActor*>(pairHeader.actors[1]->userData);
			IPhysicsCallback* otherActorCallback = static_cast<IPhysicsCallback*>(otherActor->GetCallbackData());

			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				if (actorCallback != nullptr)
				{
					actorCallback->OnCollisionEnter(otherActor);
				}
				if (otherActorCallback != nullptr)
				{
					otherActorCallback->OnCollisionEnter(actor);
				}
			}

			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				if (actorCallback != nullptr)
				{
					actorCallback->OnCollisionExit(otherActor);
				}
				if (otherActorCallback != nullptr)
				{
					otherActorCallback->OnCollisionExit(actor);
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

			CPhysicsCallbackActor* triggerActor = static_cast<CPhysicsCallbackActor*>(pairs[i].triggerActor->userData);
			IPhysicsCallback* triggerActorCallback = static_cast<IPhysicsCallback*>(triggerActor->GetCallbackData());

			CPhysicsCallbackActor* otherActor = static_cast<CPhysicsCallbackActor*>(pairs[i].otherActor->userData);
			IPhysicsCallback* otherActorCallback = static_cast<IPhysicsCallback*>(otherActor->GetCallbackData());

			if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				if (triggerActorCallback != nullptr)
				{
					triggerActorCallback->OnTriggerEnter(otherActor);
				}
				if (otherActorCallback != nullptr)
				{
					otherActorCallback->OnTriggerEnter(triggerActor);
				}
			}
			if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				if (triggerActorCallback != nullptr)
				{
					triggerActorCallback->OnTriggerExit(otherActor);
				}
				if (otherActorCallback != nullptr)
				{
					otherActorCallback->OnTriggerExit(triggerActor);
				}
			}
			if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
			{
				if (triggerActorCallback != nullptr)
				{
					triggerActorCallback->OnTriggerExit(otherActor);
				}
				if (otherActorCallback != nullptr)
				{
					otherActorCallback->OnTriggerExit(triggerActor);
				}
			}
		}
	}

	void CSimulationEventCallback::onAdvance(const physx::PxRigidBody*const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
	{

	}
}
