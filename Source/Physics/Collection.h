#pragma once
#include "CollisionLayers.h"

namespace physx
{
	class PxCollection;
}

namespace Physics
{
	class CCollection
	{
	public:
		explicit CCollection(physx::PxCollection* aCollection);
		~CCollection();
		physx::PxCollection* GetCollection();
	protected:
		physx::PxCollection* myCollection;
	};
}

