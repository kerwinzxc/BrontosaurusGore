#include "ParticleSizeUpdate.h"
#include "JsonValue.h"
#include "../Components/ParticleEmitterComponentManager.h"

Particles::CParticleSizeUpdate::CParticleSizeUpdate(const CU::CJsonValue& aJsonValue)
{
	myStart = aJsonValue["start"].GetFloat();
	myEnd = aJsonValue["end"].GetFloat();
}

void Particles::CParticleSizeUpdate::Update(const float aDt, SParticle& aParticle, SParticleLogic& aParticleLogic)
{
	const float t = aParticleLogic.lifetimeLeft / aParticleLogic.lifeTime;
	aParticle.size = myStart * t + myEnd * (1.f - t);
}

Particles::CParticleSizeUpdate::~CParticleSizeUpdate()
{
}
