#pragma once

struct SParticleLogic;
struct SParticle;

namespace Particles
{
	class IParticleSpawner
	{
	public:
		IParticleSpawner();
		virtual ~IParticleSpawner();
		virtual void Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic) = 0;
	};
}