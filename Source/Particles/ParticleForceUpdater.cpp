#include "ParticleForceUpdater.h"
#include "vector3.h"
#include "JsonValue.h"
#include "../Components/ParticleEmitterComponentManager.h"


Particles::CParticleForceUpdater::CParticleForceUpdater(const CU::CJsonValue& aJsonValue)
{
	myForce = aJsonValue["force"].GetVector3f();
}

Particles::CParticleForceUpdater::~CParticleForceUpdater()
{
}

void Particles::CParticleForceUpdater::Update(const float aDt, SParticle& aParticle, SParticleLogic& aParticleLogic)
{
	aParticleLogic.movementDir += myForce * aDt;
}

