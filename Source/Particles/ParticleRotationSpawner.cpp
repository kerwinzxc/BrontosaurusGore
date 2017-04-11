#include "ParticleRotationSpawner.h"
#include "JsonValue.h"
#include "../Components/ParticleEmitterComponentManager.h"

Particles::CParticleRotationSpawner::CParticleRotationSpawner(const CU::CJsonValue& aJsonValue)
{
	myMin = aJsonValue["min"].GetFloat();;
	myMax = aJsonValue["max"].GetFloat();
}

Particles::CParticleRotationSpawner::~CParticleRotationSpawner()
{
}

void Particles::CParticleRotationSpawner::Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic)
{
	const float t = static_cast<float>(rand()) / RAND_MAX;
	aParticle.position.w = myMin * t + myMax * (1.f - t);
}
