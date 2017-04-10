#pragma once
#include "IParticleSpawner.h"


namespace CU {
	class CJsonValue;
}

namespace Particles
{
	class CParticleVelocitySpawner : public IParticleSpawner
	{
	public:
		explicit CParticleVelocitySpawner(const CU::CJsonValue& aJsonValue);
		~CParticleVelocitySpawner();
		void Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic) override;
	private:

		float GenerateVelocity();
		float myMin;
		float myMax;
	};
}

