#include "ParticleColorSpawner.h"
#include "JsonValue.h"
#include <cstdlib>
#include "../BrontosaurusEngine/EmitterData.h"

Particles::CParticleColorSpawner::CParticleColorSpawner(const CU::CJsonValue& aJsonValue)
{
	myMin = aJsonValue["min"].GetVector4f("rgba");
	myMax = aJsonValue["max"].GetVector4f("rgba");
}

Particles::CParticleColorSpawner::~CParticleColorSpawner()
{
}

void Particles::CParticleColorSpawner::Spawn(SParticle& aParticle, SParticleLogic& aParticleLogic)
{
	aParticle.color = GenerateColor();
}

CU::Vector4f Particles::CParticleColorSpawner::GenerateColor()
{
	//TODO: Add support for different randomization
	return myMin + (myMax - myMin) * static_cast<float>(rand()) / RAND_MAX;
}
