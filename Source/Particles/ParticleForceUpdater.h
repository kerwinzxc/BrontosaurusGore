#pragma once
#include "IParticleUpdater.h"
#include "vector3.h"

namespace CU {
	class CJsonValue;
}

namespace Particles
{
	class CParticleForceUpdater : public IParticleUpdater
	{
	public:
		CParticleForceUpdater(const CU::CJsonValue& aJsonValue);
		~CParticleForceUpdater();

		void Update(const float aDt, SParticle& aParticle, SParticleLogic& aParticleLogic) override;
	private:
		CU::Vector3f myForce;
	};
}

