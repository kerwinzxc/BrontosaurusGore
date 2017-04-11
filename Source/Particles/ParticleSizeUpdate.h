#pragma once
#include "IParticleUpdater.h"

namespace CU {
	class CJsonValue;
}

namespace Particles
{
	class CParticleSizeUpdate : public IParticleUpdater
	{
	public:
		CParticleSizeUpdate(const CU::CJsonValue& aJsonValue);
		~CParticleSizeUpdate();
		void Update(const float aDt, SParticle& aParticle, SParticleLogic& aParticleLogic) override;
	private:
		float myStart;
		float myEnd;
	};
}

