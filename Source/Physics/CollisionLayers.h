#pragma once
#include "foundation\PxSimpleTypes.h"

namespace Physics
{
	enum ECollisionLayer : physx::PxU32
	{
		eDefault = 1 << 0,
		// 1 - 7 unity reserved idk

		ePlayer  = 1 << 8,
		eEnemy = 1 << 9,
		eProjectile = 1 << 10,
	};
}
