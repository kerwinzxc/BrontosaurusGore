#pragma once
#include "ParticleEmitterComponent.h"
#include "../BrontosaurusEngine/EmitterData.h"


class CScene;

namespace CU
{
	class Time;
}

class CParticleEmitterComponentManager
{
public:
	static CParticleEmitterComponentManager& GetInstance();

	static void Create();
	static void Destroy();

	CParticleEmitterComponent* CreateComponent(const std::string& aSystemId);
	
	void RemoveAll();
	void Remove(CParticleEmitterComponent* aComponentToRemove);

	void SetScene(CScene* aScene);

	void UpdateEmitters(const CU::Time& aDeltaTime);
private:
	CParticleEmitterComponentManager();
	~CParticleEmitterComponentManager();

private:
	static CParticleEmitterComponentManager* myInstance;
	CU::GrowingArray<CParticleEmitterComponent*> myComponents;

	CScene* myScene;

};

