#pragma once
#include "IParticleSpawner.h"

namespace CU {
	class CJsonValue;
}

namespace Particles
{
	class CParticleSizeSpawner : public IParticleSpawner
	{
	public:
		explicit CParticleSizeSpawner(const CU::CJsonValue& aJsonValue);
		~CParticleSizeSpawner();
		
		void Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic) override;
	private:
		float GenerateSize();
		float myMin;
		float myMax;
	};
}

