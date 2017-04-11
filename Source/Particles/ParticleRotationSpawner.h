#pragma once
#include "IParticleSpawner.h"

namespace CU {
	class CJsonValue;
}

namespace Particles
{
	class CParticleRotationSpawner : public IParticleSpawner
	{
	public:
		CParticleRotationSpawner(const CU::CJsonValue& aJsonValue);
		~CParticleRotationSpawner();
		void Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic) override;
	private:
		float myMin;
		float myMax;
	};
}

