#pragma once
#include "IParticleSpawner.h"
#include "../CommonUtilities/vector4.h"

namespace CU {
	class CJsonValue;
}

namespace Particles
{ 
	class CParticleColorSpawner: public IParticleSpawner
	{
	public:
		explicit CParticleColorSpawner(const CU::CJsonValue& aJsonValue);
		~CParticleColorSpawner();
		void Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic) override;
	private:
		CU::Vector4f GenerateColor();
		CU::Vector4f myMin;
		CU::Vector4f myMax;
	};
}

