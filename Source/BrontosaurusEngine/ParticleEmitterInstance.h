#pragma once
#include "matrix44.h"
#include "InstanceID.h"
#include "EmitterData.h"

namespace CU
{
	class Time;
	class Camera;
}

class CParticleEmitter;

typedef unsigned int ParticleEmitterID;

class CParticleEmitterInstance
{
public:
	CParticleEmitterInstance(const SEmitterData& aEmitterData);

	~CParticleEmitterInstance();

	void Update(const CU::Time& aDeltaTime);
	void Render(const CU::Camera& aCamera); // needs the camera to sort by
	void Activate();
	void Deactivate();
	static void DistanceSort(CU::GrowingArray<SParticle, unsigned int, false>& aParticleList, const CU::Camera & aCamera);
	inline void SetPosition(CU::Vector3f aPosition);
	inline void SetVisibility(bool);
	inline bool IsVisible() const;

	inline InstanceID GetInstanceID();
	inline void SetInstandeID(const InstanceID aID);

	bool IsDone() const;

private:
	void Init();
	void EmitParticle();


private:
	CU::Matrix44f	myToWorldSpace;
	SEmitterData	myEmitterData;


	CU::GrowingArray<SParticle, unsigned int, false> myParticles;
	CU::GrowingArray<SParticleLogic> myParticleLogic;

	float myEmitDelta;
	float myEmitTimer;
	ParticleEmitterID myEmitterID;
	InstanceID myInstanceID;


	bool  myIsActive;
	bool myIsVisible;
	float myLifetime;
};

inline void CParticleEmitterInstance::SetPosition(CU::Vector3f aPosition)
{
	myToWorldSpace.SetPosition(aPosition);
}

inline void CParticleEmitterInstance::SetVisibility(bool aValue)
{
	myIsVisible = aValue;
}

inline bool CParticleEmitterInstance::IsVisible() const
{
	return myIsVisible;
}

inline InstanceID CParticleEmitterInstance::GetInstanceID()
{
	return myInstanceID;
}

inline void CParticleEmitterInstance::SetInstandeID(const InstanceID aID)
{
	myInstanceID = aID;
}

inline bool CParticleEmitterInstance::IsDone() const
{
	return myParticles.Empty();
}

