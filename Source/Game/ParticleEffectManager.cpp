#include "stdafx.h"
#include "ParticleEffectManager.h"
#include "vector3.h"
#include "../BrontosaurusEngine/Scene.h"
#include "../BrontosaurusEngine/ParticleEmitterInstance.h"
#include "../BrontosaurusEngine/EmitterData.h"

#include "../CommonUtilities/AlarmClock.h"
#include "CommonUtilities/JsonValue.h"

CParticleEffectManager* CParticleEffectManager::ourInstance = nullptr;

struct CParticleEffectManager::SParticleData
{
	SEmitterData myInitData = {};
	CU::CAlarmClock myAlarmClock;
	CU::Time myActiveTime = 0.f;
};

CParticleEffectManager::CParticleEffectManager(CScene& aScene)
	: myActiveParticleEmitters(16u)
	, myKilledParticleEmitters(16u)
	, myFreeEmitterIDs(16u)
	, myScene(aScene)
{
	assert(ourInstance == nullptr && "particle effect manager already created");
	ourInstance = this;

	LoadParticleData();
	
	assert(myParticleData.empty() == false && "Load the data because i can't find any blood");
}

CParticleEffectManager::~CParticleEffectManager()
{
	assert(ourInstance == this && "another particle effect manager created");
	ourInstance = nullptr;
}

void CParticleEffectManager::SpawnParticle(const std::string& aEffectName, const CU::Vector3f& aSpawnPosition)
{
	unsigned int activeParticlesIndex = myActiveParticleEmitters.Size();
	if (!myFreeEmitterIDs.Empty())
	{
		activeParticlesIndex = myFreeEmitterIDs.Pop();
	}
	else
	{
		myActiveParticleEmitters.Add(SParticleData());
	}

	SParticleData& data = myActiveParticleEmitters[activeParticlesIndex];

	if (!GetEmitterData(aEffectName, data))
	{
		if (&data == &myActiveParticleEmitters.GetLast())
		{
			myActiveParticleEmitters.Pop();
		}

		return;
	}


	CParticleEmitterInstance* particleEmitter = new CParticleEmitterInstance(data.myInitData);
	particleEmitter->SetPosition(aSpawnPosition);
	InstanceID particleID = myScene.AddParticleEmitterInstance(particleEmitter);
	
	data.myAlarmClock.Init(data.myActiveTime, std::bind(&CParticleEffectManager::DespawnParticle, this, particleID, activeParticlesIndex));

}

void CParticleEffectManager::DespawnParticle(const InstanceID aEmitterIndex, const unsigned int aParticleDataIndex)
{
	SParticleData& particle = myActiveParticleEmitters[aParticleDataIndex];
	//myScene.DeleteParticleEmitterInstance(aEmitterIndex); //don't delete, just stop spawning and move to to-be-deleted-list
	CParticleEmitterInstance* emitter = myScene.GetParticleEmitterInstance(aEmitterIndex);
	if (emitter)
	{
		emitter->Deactivate();
		myKilledParticleEmitters.Add(aEmitterIndex);
	
	}
	
	myActiveParticleEmitters[aParticleDataIndex] = SParticleData();
	myFreeEmitterIDs.Add(aParticleDataIndex);
}

void CParticleEffectManager::Update()
{
	for (SParticleData& data : myActiveParticleEmitters)
	{
		data.myAlarmClock.Update();
	}

	for (unsigned int i = 0; i < myKilledParticleEmitters.Size(); ++i)
	{
		InstanceID id = myKilledParticleEmitters[i];
		CParticleEmitterInstance* emitter = myScene.GetParticleEmitterInstance(id);
		if (emitter && emitter->IsDone())
		{
			myScene.DeleteParticleEmitterInstance(id);
			myKilledParticleEmitters.RemoveCyclicAtIndex(i);

			--i;
		}
	}
}

CParticleEffectManager* CParticleEffectManager::GetInstance()
{
	return ourInstance;
}

void CParticleEffectManager::LoadParticleData()
{
	CU::CJsonValue particleEffectDocument("Json/Particles/ParticleEffects.json");
	CU::CJsonValue effectsArray = particleEffectDocument["Effects"];

	for (int i = 0; i < effectsArray.Size(); ++i)
	{
		const std::string& name = effectsArray[i]["name"].GetString();
		if (name.empty()) continue;

		SParticleData data;

		const std::string& emitterJsonPath = effectsArray[i]["initData"].GetString();
		if (!LoadParticleData(emitterJsonPath, data.myInitData)) continue;

		float activeSeconds = effectsArray[i]["activeTime"].GetFloat();
		data.myActiveTime.SetSeconds(activeSeconds);
		
		myParticleData[name] = data;
	}
}

bool CParticleEffectManager::LoadParticleData(const std::string& aEffectPath, SEmitterData& aEmitterDataOut)
{
	CU::CJsonValue particleEffectDocument(aEffectPath);
	if (particleEffectDocument.IsNull()) return false;

	aEmitterDataOut.TexturePath = particleEffectDocument["TexturePath"].GetString().c_str();

	aEmitterDataOut.StartColor = particleEffectDocument["StartColor"].GetVector4f("rgba");
	aEmitterDataOut.EndColor = particleEffectDocument["StartColor"].GetVector4f("rgba");

	aEmitterDataOut.MinEmissionVelocity = particleEffectDocument["MinEmissionVelocity"].GetVector3f();
	aEmitterDataOut.MaxEmissionVelocity = particleEffectDocument["MaxEmissionVelocity"].GetVector3f();

	aEmitterDataOut.MinEmissionArea = particleEffectDocument["MinEmissionArea"].GetVector3f();
	aEmitterDataOut.MaxEmissionArea = particleEffectDocument["MaxEmissionArea"].GetVector3f();

	aEmitterDataOut.Gravity = particleEffectDocument["Gravity"].GetVector3f();

	aEmitterDataOut.StartSize = particleEffectDocument["StartSize"].GetFloat();
	aEmitterDataOut.EndSize = particleEffectDocument["EndSize"].GetFloat();
	aEmitterDataOut.EmissionRate = particleEffectDocument["EmissionRate"].GetFloat();
	aEmitterDataOut.StartRotation = particleEffectDocument["StartRotation"].GetFloat();
	aEmitterDataOut.EndRotation = particleEffectDocument["EndRotation"].GetFloat();
	aEmitterDataOut.MinParticleLifeTime = particleEffectDocument["MinParticleLifeTime"].GetFloat();
	aEmitterDataOut.MaxParticleLifeTime = particleEffectDocument["MaxParticleLifeTime"].GetFloat();

	CU::CJsonValue colorOverLifeArray = particleEffectDocument["ColorOverLife"];
	for (int i = 0; i < colorOverLifeArray.Size(); ++i)
	{
		aEmitterDataOut.ColorOverLife[i].time = colorOverLifeArray[i]["time"].GetFloat();
		aEmitterDataOut.ColorOverLife[i].value = colorOverLifeArray[i]["value"].GetVector4f("rgba");
	}

	aEmitterDataOut.NumOfParticles = particleEffectDocument["NumberOfParticles"].GetInt();

	aEmitterDataOut.RotationCurve = particleEffectDocument["RotationCurve"].GetEnum<eLerpCurve>();
	aEmitterDataOut.ColorCurve = particleEffectDocument["ColorCurve"].GetEnum<eLerpCurve>();
	aEmitterDataOut.SizeCurve = particleEffectDocument["SizeCurve"].GetEnum<eLerpCurve>();

	aEmitterDataOut.UseGravity = particleEffectDocument["UseGravity"].GetBool();

	return true;
}

bool CParticleEffectManager::GetEmitterData(const std::string& aEffectName, SParticleData& aDataOut) const
{
	auto it = myParticleData.find(aEffectName);
	if (it == myParticleData.end())
	{
		return false;
	}

	aDataOut = it->second;
	return true;
}
