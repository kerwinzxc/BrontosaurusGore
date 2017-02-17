#pragma once

namespace physx
{
	class PxFoundation;
	class PxPhysics;
	class PxPvd;
}

class CPhysXManager
{
public:
	CPhysXManager();
	~CPhysXManager();

private:
	physx::PxFoundation* myFoundation;
	physx::PxPhysics* myPhysics;
	physx::PxPvd* myPvd;
};

