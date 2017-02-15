#include "stdafx.h"
#include "StreakEmitterInstance.h"
#include "StreakEmitter.h"
#include "EmitterData.h"


CStreakEmitterInstance::CStreakEmitterInstance()
{
	SStreakEmitterData emitterData;

	emitterData.TexturePath = "Sprites/Streak.dds";

	emitterData.EmissionRate = 400.f;

	emitterData.StartAlpha = 1.0f;
	emitterData.EndAlpha = 1.0f;

	emitterData.StartSize = 15.0f;
	emitterData.EndSize = 0.0f;

	emitterData.ParticleLifetime = 0.25f;

	emitterData.NumOfParticles = 64;
	emitterData.EmissonLifeTime = -1.f;

	myEmitter = new CStreakEmitter(emitterData);
	myEmitter->Init();
}

CStreakEmitterInstance::CStreakEmitterInstance(const SStreakEmitterData & aEmitterValues)
{
	myEmitter = new CStreakEmitter(aEmitterValues);
	myEmitter->Init();

}

CStreakEmitterInstance::~CStreakEmitterInstance()
{
	delete myEmitter;
	myEmitter = nullptr;
}


void CStreakEmitterInstance::Update(const CU::Time& aDeltaTime)
{
	myEmitter->Update(aDeltaTime, myToWorldSpace);
}

void CStreakEmitterInstance::Render()
{
	myEmitter->Render(myToWorldSpace);
}

void CStreakEmitterInstance::Activate()
{
	myEmitter->Activate();
}

void CStreakEmitterInstance::Deactivate()
{
	myEmitter->Deactivate();
}


bool CStreakEmitterInstance::IsVisible() const
{
	return myEmitter->IsVisible();
}
