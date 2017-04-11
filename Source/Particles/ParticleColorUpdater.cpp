#include "ParticleColorUpdater.h"
#include "JsonValue.h"
#include "../Components/ParticleEmitterComponentManager.h"

Particles::CParticleColorUpdater::CParticleColorUpdater(const CU::CJsonValue& aJsonValue)
{
	myStart = aJsonValue["start"].GetVector4f("rgba");
	myEnd = aJsonValue["end"].GetVector4f("rgba");
}

void Particles::CParticleColorUpdater::Update(const float aDt, SParticle& aParticle, SParticleLogic& aParticleLogic)
{
	const float t = aParticleLogic.lifetimeLeft / aParticleLogic.lifeTime;
	aParticle.color = myStart * t + myEnd * (1.f - t);
}

Particles::CParticleColorUpdater::~CParticleColorUpdater()
{
}
