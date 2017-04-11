#pragma once
#include "IParticleUpdater.h"
#include "../CommonUtilities/vector4.h"

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
	protected:
		CU::Vector4f myStart;
		CU::Vector4f myEnd;
	};
}

