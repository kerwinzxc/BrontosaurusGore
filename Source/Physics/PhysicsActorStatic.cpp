#include "stdafx.h"
#include "PhysicsActorStatic.h"
#include "PxShape.h"
#include "PxRigidStatic.h"

namespace Physics
{

	CPhysicsActorStatic::CPhysicsActorStatic(physx::PxRigidStatic* aActor, CShape* aShape)
		: CPhysicsActor(aActor, aShape)
	{
		myStaticActor = aActor;
	}

	CPhysicsActorStatic::~CPhysicsActorStatic()
	{
		myStaticActor = nullptr;
	}
}