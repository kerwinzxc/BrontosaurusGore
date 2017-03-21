#pragma once

namespace Physics
{
	enum ECollisionLayer : unsigned int
	{
		eDefault = 1 << 0,
		// 1 - 7 unity reserved idk

		ePlayer  = 1 << 8,
		eEnemy = 1 << 9,
		eProjectile = 1 << 10,

	};

	static const unsigned int CollideEverything = ~0;

}
