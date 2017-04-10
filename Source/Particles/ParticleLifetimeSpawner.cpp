#include "ParticleLifetimeSpawner.h"
#include "JsonValue.h"
#include "../BrontosaurusEngine/EmitterData.h"


Particles::CParticleLifetimeSpawner::CParticleLifetimeSpawner(const CU::CJsonValue& aJsonValue)
{
	myMin = aJsonValue["min"].GetFloat();
	myMax = aJsonValue["max"].GetFloat();
}

Particles::CParticleLifetimeSpawner::~CParticleLifetimeSpawner()
{
}

void Particles::CParticleLifetimeSpawner::Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic)
{
	//TODO: Handle different kinds of randomization
	aParticleLogic.lifeTime = myMin + (myMax - myMin) * static_cast<float>(rand()) / RAND_MAX;
	aParticleLogic.lifetimeLeft = aParticleLogic.lifeTime;
}
