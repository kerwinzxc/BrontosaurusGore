#pragma once

struct SParticleLogic;
struct SParticle;

namespace Particles
{
	class IParticleUpdater
	{
	public:

		IParticleUpdater(){}

		virtual ~IParticleUpdater(){}

		virtual void Update(const float aDt, SParticle& aParticle, SParticleLogic& aParticleLogic) = 0;
	};
}

