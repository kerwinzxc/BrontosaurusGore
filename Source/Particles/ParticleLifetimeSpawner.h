#pragma once
#include "IParticleSpawner.h"

namespace CU {
	class CJsonValue;
}

namespace Particles
{
	class CParticleLifetimeSpawner : public IParticleSpawner
	{
	public:
		CParticleLifetimeSpawner(const CU::CJsonValue& aJsonValue);
		~CParticleLifetimeSpawner();
		void Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic) override;
	private:
		float myMin;
		float myMax;
	};
}

