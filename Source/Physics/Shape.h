#pragma once
#include "CollisionLayers.h"

namespace physx
{
	class PxShape;
}

namespace Physics
{

	//enum ECollisionLayer;

	class CShape
	{
		friend class CPhysics;
		friend class CPhysicsActor;

	public:
		CShape(physx::PxShape* aShape);
		CShape(const CShape& aShape);
		~CShape();

		void SetCollisionLayers(const ECollisionLayer& aLayer);
		void SetCollisionLayers(const ECollisionLayer& aLayer, const ECollisionLayer& aLayerToCollideWith);
		void SetObjectId(int aI);
	private:
		physx::PxShape* myShape;

	};
}
