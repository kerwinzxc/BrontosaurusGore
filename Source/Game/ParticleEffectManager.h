#pragma once
#include <map>

class CScene;
using InstanceID = unsigned int;

class CParticleEffectManager
{
public:
	CParticleEffectManager(CScene& aScene);
	~CParticleEffectManager();

	void SpawnParticle(const std::string& aEffectName, const CU::Vector3f& aSpawnPosition);
	void DespawnParticle(const InstanceID aEmitterIndex, const unsigned int aParticleDataIndex);
	void Update();

	static CParticleEffectManager* GetInstance();

private:
	struct SParticleData;

	void LoadParticleData();
	bool LoadParticleData(const std::string& aEffectPath, struct SEmitterData& aEmitterDataOut);
	bool GetEmitterData(const std::string& aEffectName, SParticleData& aDataOut) const;

	std::map<std::string, SParticleData> myParticleData;

	CU::GrowingArray<SParticleData> myActiveParticleEmitters;
	CU::GrowingArray<InstanceID> myKilledParticleEmitters;
	CU::GrowingArray<unsigned int> myFreeEmitterIDs;

	CScene& myScene;

	static CParticleEffectManager* ourInstance;
};

