#pragma once
#include "IParticleUpdater.h"

namespace CU {
	class CJsonValue;
}

namespace Particles
{
	class CParticleVelocityUpdater : public IParticleUpdater
	{
	public:
		explicit CParticleVelocityUpdater(const CU::CJsonValue& aJsonValue);
		~CParticleVelocityUpdater();
	};
}

