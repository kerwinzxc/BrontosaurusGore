#pragma once
#include "../CommonUtilities/Stack.h"
#include "../Particles/ParticleLibrary.h"

class CParticleEmitter;
struct SEmitterData;


class CParticleEmitterManager
{
public:
	typedef int ParticleEmitterID;
	static CParticleEmitterManager& GetInstance();
	static void Create();
	static void Destroy();

	const ParticleEmitterID CreateParticleEmitter(const SEmitterData& aEmitterData);
	CParticleEmitter* GetParticleEmitter(const ParticleEmitterID aID);

	void RemoveParticleEmitter(const ParticleEmitterID aID);
	void LoadParticleLibrary(const std::string& aLibraryPath);
private:

	CParticleEmitterManager();
	~CParticleEmitterManager();

	struct SParticleEmitterComparer
	{
		std::string myTexturePath;
		unsigned int myNumOfParticles;
		ParticleEmitterID myID;

		int myRefCount;

		SParticleEmitterComparer()
		{
			myTexturePath = "";
			myNumOfParticles = 0;
			myRefCount = 0;
			myID = -1;
		}

		bool operator==(const SEmitterData& aEmitterData);
	};

	//return -1 if no emitter exists
	int EmitterExists(const SEmitterData& aEmitterData);
	void AddRef(const ParticleEmitterID aID);
	void RemoveRef(const ParticleEmitterID aID);



private:
	CU::GrowingArray<CParticleEmitter> myParticleEmitters;
	CU::Stack<ParticleEmitterID> myFreeParticleEmitterIDs;
	CU::GrowingArray<SParticleEmitterComparer> myParticleEmitterComp;
	std::map<std::string, Particles::CParticleLibrary> myLibraries;
	static CParticleEmitterManager* ourInstance;
};

