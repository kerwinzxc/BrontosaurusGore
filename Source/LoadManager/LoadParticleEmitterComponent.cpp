#include "stdafx.h"
#include "LoadParticleEmitterComponent.h"
#include "ParticleEmitterComponentManager.h"

int LoadParticleEmitterComponent(KLoader::SLoadedComponentData someData)
{
	SEmitterData emitterData;
	/*emitterData.UseGravity = someData.myData["UseGravity"].GetBool();

	if(emitterData.UseGravity == true)
	{
		emitterData.Gravity = someData.myData["Gravity"].GetVector3f();
	}

	emitterData.EmissionRate = someData.myData["EmissionRate"].GetFloat();
	emitterData.NumOfParticles = someData.myData["MaxParticles"].GetInt();

	emitterData.StartSize =	someData.myData["StartSize"].GetFloat();
	emitterData.EndSize =	someData.myData["EndSize"].GetFloat();

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

	emitterData.ColorCurve = eLerpCurve::eLinear;
	emitterData.SizeCurve = eLerpCurve::eLinear;
	emitterData.SizeCurve = eLerpCurve::eLinear;*/
	CParticleEmitterComponent* companent = CParticleEmitterComponentManager::GetInstance().CreateComponent(someData.myData["ParticleEmitterName"].GetString());
	return companent->GetId();
}
