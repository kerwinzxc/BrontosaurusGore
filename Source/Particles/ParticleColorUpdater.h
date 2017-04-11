#pragma once
#include "IParticleUpdater.h"

namespace CU {
	class CJsonValue;
}

namespace Particles
{
	class CParticleColorUpdater : public IParticleUpdater
	{
	public:
		CParticleColorUpdater(const CU::CJsonValue& aJsonValue);
		~CParticleColorUpdater();
		void Update(const float aDt, SParticle& aParticle, SParticleLogic& aParticleLogic) override;
	};
}

