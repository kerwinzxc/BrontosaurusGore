#include "ParticleFrictionUpdater.h"
#include "JsonValue.h"
#include "../Components/ParticleEmitterComponentManager.h"

Particles::CParticleFrictionUpdater::CParticleFrictionUpdater(const CU::CJsonValue& aJsonValue)
{
	myFriction = aJsonValue["value"].GetFloat();
}

Particles::CParticleFrictionUpdater::~CParticleFrictionUpdater()
{
}

void Particles::CParticleFrictionUpdater::Update(const float aDt, SParticle& aParticle, SParticleLogic& aParticleLogic)
{
	aParticleLogic.movementDir *= (1.f - myFriction * aDt);
}
