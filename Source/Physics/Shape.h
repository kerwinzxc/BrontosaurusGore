#pragma once

namespace physx
{
	class PxShape;
}

namespace Physics
{
	class CShape
	{
		friend class CPhysics;
		friend class CPhysicsActor;

	public:
		CShape(physx::PxShape* aShape);
		CShape(const CShape& aShape);

		~CShape();
	private:
		physx::PxShape* myShape;

	};
}
