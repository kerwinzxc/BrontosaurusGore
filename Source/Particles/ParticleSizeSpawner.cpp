#include "ParticleSizeSpawner.h"
#include "../BrontosaurusEngine/EmitterData.h"
#include "JsonValue.h"

Particles::CParticleSizeSpawner::CParticleSizeSpawner(const CU::CJsonValue& aJsonValue)
{
	myMin = aJsonValue["min"].GetFloat();
	myMax = aJsonValue["max"].GetFloat();
}

Particles::CParticleSizeSpawner::~CParticleSizeSpawner()
{
}

void Particles::CParticleSizeSpawner::Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic)
{
	aParticle.size = GenerateSize();
}

float Particles::CParticleSizeSpawner::GenerateSize()
{
	//TODO: Add support for different randomization
	return myMin + (myMax - myMin) * static_cast<float>(rand()) / RAND_MAX;
}
