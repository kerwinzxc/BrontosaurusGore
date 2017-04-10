#pragma once
#include "../CommonUtilities/matrix44.h"
#include "InstanceID.h"
#include "EmitterData.h"
#include "../CommonUtilities/GrowingArray.h"

namespace CU
{
	class Time;
	class Camera;
}

class CParticleEmitter;
class CRenderCamera;

typedef unsigned int ParticleEmitterID;

class CParticleEmitterInstance
{
public:
	friend class CParticleEmitter;

	
	CParticleEmitterInstance(int anId);
	~CParticleEmitterInstance();

	void Update(const CU::Time& aDeltaTime);
	void Render(CRenderCamera& aRenderCamera); // needs the camera to sort by
	void Activate();
	void Deactivate();
	inline void SetPosition(CU::Vector3f aPosition);
	inline void SetVisibility(bool);
	inline bool IsVisible() const;

	inline InstanceID GetInstanceID();
	inline void SetInstandeID(const InstanceID aID);

	bool IsDone() const;
	int GetEmitterId();
	void ResetLifetime();
	void ResetSpawnTimer();
	bool IsActive();
	void SetTransformation(const CU::Matrix44f& aMatrix44);
private:
	void Init();
	void EmitParticle();
	static void DistanceSort(CU::GrowingArray<SParticle, unsigned int, false>& aParticleList, const CU::Camera & aCamera);


private:
	CU::Matrix44f	myToWorldSpace;

	CU::GrowingArray<SParticle, unsigned int, false> myParticles;
	CU::GrowingArray<SParticleLogic> myParticleLogic;

	float myEmitTimer;
	int myEmitterID;
	InstanceID myInstanceID;


	bool  myIsActive;
	bool myIsVisible;
	float myLifetime;
	static int ourIds;
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

