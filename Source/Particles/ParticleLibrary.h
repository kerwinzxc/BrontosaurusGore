#pragma once
#include <string>
#include <map>
#include "../Particles/Typedefs.h"
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

		CParticleLibrary();
		~CParticleLibrary();
		
		void Load(const std::string& aPath);
		Particles::ParticleEmitterID AddRef(const std::string& aSystemName);
		void AddRef(const Particles::ParticleEmitterID anId);
		void RemoveRef(const std::string& aSystemName);
		void RemoveRef(const Particles::ParticleEmitterID anId);
		Particles::ParticleEmitterID GetSystem(const std::string& aSystemId);
		CParticleEmitter* GetSystemP(ParticleEmitterID aParticleEmitter);
		static const Particles::ParticleEmitterID FoundNone;
	private:
		void AddSystemRef(const std::string& aName, const Particles::ParticleEmitterID aId);
		void LoadSystem(const CU::CJsonValue& aJsonValue);
		
		std::map<std::string, Particles::ParticleEmitterID> myStringIdMap;
		std::map<Particles::ParticleEmitterID, CParticleEmitter*> myIdEmitterMap;
		
#ifndef _RETAIL_BUILD

		std::string myFile;
		void Reload();
		void WatchFile(const std::string& aPath);
		CFileWatcher myFileWatcher;
#endif
	};
}

