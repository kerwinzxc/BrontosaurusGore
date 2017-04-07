#pragma once
#include <string>
#include <map>
#include "../BrontosaurusEngine/ParticleEmitter.h"

namespace CU
{
	class CJsonValue;
}

namespace Particles
{
	class CParticleLibrary
	{
	public:

		
		using ParticleId = int;
		CParticleLibrary();
		~CParticleLibrary();
		
		void Load(const std::string& aPath);
		ParticleId AddRef(const std::string& aSystemName);
		void AddRef(const ParticleId anId);
		void RemoveRef(const std::string& aSystemName);
		void RemoveRef(const ParticleId anId);
	private:
		void AddSystemRef(const std::string& aName, const ParticleId aId);
		void LoadSystem(const CU::CJsonValue& aJsonValue);
		std::map<std::string, ParticleId> myStringIdMap;
		
		std::map<ParticleId, CParticleEmitter> myIdEmitterMap;
	};
}

