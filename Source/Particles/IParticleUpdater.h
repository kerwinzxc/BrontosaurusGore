#pragma once
#include "../Components/ParticleEmitterComponent.h"

namespace Particles
{
	class IParticleUpdater
	{
	public:

		IParticleUpdater(){}

		virtual ~IParticleUpdater(){}

		virtual void Update(const float aDt, CParticleEmitterInstance& anParticleEmitterInstance) = 0;
	};
}

