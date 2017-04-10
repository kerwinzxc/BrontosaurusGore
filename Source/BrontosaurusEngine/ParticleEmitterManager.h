#pragma once
#include "../CommonUtilities/Stack.h"
#include <map>
#include <vector>
#include "../CommonUtilities/GrowingArray.h"
#include "../Particles/Typedefs.h"
#include "RenderCamera.h"
#include "../CommonUtilities/CUTime.h"
#include "../CommonUtilities/TimerManager.h"

namespace Particles {
	class CParticleLibrary;
}

class CParticleEmitterInstance;
class CParticleEmitter;
struct SEmitterData;


class CParticleEmitterManager
{
public:
	static CParticleEmitterManager& GetInstance();
	static void Create();
	static void Destroy();

	/*const ParticleEmitterID CreateParticleEmitter(const SEmitterData& aEmitterData);
	CParticleEmitter* GetParticleEmitter(const ParticleEmitterID aID);*/

	void RemoveParticleEmitter(const Particles::ParticleEmitterID aID);
	void LoadParticleLibrary(const std::string& aLibraryPath);
	Particles::ParticleEmitterID GetEmitterInstance(Particles::ParticleEmitterID aId);
	Particles::ParticleEmitterID GetParticleEmitterId(const std::string& aSystemId);

	void Release(Particles::ParticleEmitterID anInstanceId);
	void SetPosition(int aInstanceId, const CU::Vector3f& aPosition);
	void Activate(Particles::ParticleEmitterID aInstanceId);
	void Deactivate(Particles::ParticleEmitterID aInstanceId);
	CParticleEmitter* GetEmitter(Particles::ParticleEmitterID aParticleEmitter);

	void Update(const CU::Time aDeltaTime);
	void Render(CRenderCamera& aRenderCamera);

	void Run();
	bool IsActive() const;
	void SetTransformation(int aInstanceId, const CU::Matrix44f& aMatrix44);
private:
	void ThreadFinished();
	void InternalRun();
	CU::GrowingArray<CParticleEmitterInstance*>& GetFreeInstances(Particles::ParticleEmitterID aId);

	CParticleEmitterManager();
	~CParticleEmitterManager();

	struct SParticleEmitterComparer
	{
		std::string myTexturePath;
		unsigned int myNumOfParticles;
		Particles::ParticleEmitterID myID;

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
	
private:
	CU::GrowingArray<CParticleEmitterInstance*> myInstances;
	std::map<Particles::ParticleEmitterID, CU::GrowingArray<CParticleEmitterInstance*>> myFreeInstances;

	CU::GrowingArray<SParticleEmitterComparer> myParticleEmitterComp;
	std::map<std::string, Particles::CParticleLibrary*> myLibraries;
	volatile bool myIsActive;
	CU::TimerManager myTimerManager;
	static CParticleEmitterManager* ourInstance;

	volatile bool myThreadFinished;
};

