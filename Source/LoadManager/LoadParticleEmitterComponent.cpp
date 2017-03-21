#include "stdafx.h"
#include "LoadParticleEmitterComponent.h"
#include "ParticleEmitterComponentManager.h"

int LoadParticleEmitterComponent(KLoader::SLoadedComponentData someData)
{
	SEmitterData emitterData;
	emitterData.UseGravity = someData.myData["UseGravity"].GetBool();

	if(emitterData.UseGravity == true)
	{
		emitterData.Gravity = someData.myData["Gravity"].GetVector3f();
	}

	emitterData.EmissionRate = someData.myData["EmissionRate"].GetFloat();
	emitterData.NumOfParticles = someData.myData["MaxParticles"].GetInt();

	emitterData.StartSize =	someData.myData["StartSize"].GetFloat();
	emitterData.EndSize =	someData.myData["EndSize"].GetFloat() / 2.5;

	emitterData.MinParticleLifeTime = someData.myData["MinParticleLifetime"].GetFloat();
	emitterData.MaxParticleLifeTime = someData.myData["MaxParticleLifetime"].GetFloat();

	emitterData.StartColor = someData.myData["StartColor"].GetVector4f("rgba");
	emitterData.EndColor = someData.myData["EndColor"].GetVector4f("rgba");

	emitterData.MaxEmissionArea = someData.myData["MaxEmissionArea"].GetVector3f();
	emitterData.MinEmissionArea = someData.myData["MinEmissionArea"].GetVector3f();

	emitterData.MinEmissionVelocity = someData.myData["MinVelocity"].GetVector3f();
	emitterData.MaxEmissionVelocity = someData.myData["MaxVelocity"].GetVector3f();
	emitterData.TexturePath = "Models/Textures/T_M_Rock_10m_RMA.dds";

	emitterData.ShouldLoop = true;
	emitterData.Lifetime = 5.f;
	CParticleEmitterComponent* companent = CParticleEmitterComponentManager::GetInstance().CreateComponent(emitterData);
	return companent->GetId();
}
