#pragma once
#include "IParticleUpdater.h"

namespace CU {
	class CJsonValue;
}

namespace Particles
{
	class CParticleFrictionUpdater : public IParticleUpdater
	{
	public:
		CParticleFrictionUpdater(const CU::CJsonValue& aJsonValue);
		~CParticleFrictionUpdater();
		void Update(const float aDt, SParticle& aParticle, SParticleLogic& aParticleLogic) override;
	private:
		float myFriction;
	};
}

