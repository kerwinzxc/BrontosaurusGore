#pragma once

namespace physx
{
	class PxPhysics;
	class PxCooking;
	class PxPvd;
}

namespace Physics
{
	class CApex
	{
	public:
		struct SApexInit
		{
			physx::PxPhysics* physics;
			physx::PxCooking* cooking;
			physx::PxPvd* pvd;
		};

		explicit CApex(const SApexInit& anApexInit);
		~CApex();
	protected:

	};
}

