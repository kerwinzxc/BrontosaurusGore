#include "ParticleVelocitySpawner.h"
#include "../BrontosaurusEngine/EmitterData.h"
#include "JsonValue.h"

Particles::CParticleVelocitySpawner::CParticleVelocitySpawner(const CU::CJsonValue& aJsonValue)
{
	myMin = aJsonValue["min"].GetFloat();
	myMax = aJsonValue["max"].GetFloat();
}

Particles::CParticleVelocitySpawner::~CParticleVelocitySpawner()
{
}

void Particles::CParticleVelocitySpawner::Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic)
{
	aParticleLogic.speed = 1.f;
	aParticleLogic.movementDir *= GenerateVelocity();
}

float Particles::CParticleVelocitySpawner::GenerateVelocity()
{
	//TODO: Add support for different randomization
	return myMin + (myMax - myMin) * static_cast<float>(rand()) / RAND_MAX;
}
