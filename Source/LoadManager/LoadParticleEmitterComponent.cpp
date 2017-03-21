#include "stdafx.h"
#include "LoadParticleEmitterComponent.h"
#include "ParticleEmitterComponentManager.h"

int LoadParticleEmitterComponent(KLoader::SLoadedComponentData someData)
{
	SEmitterData emitterData;
	emitterData.UseGravity = true;
	emitterData.Gravity = CU::Vector3f(0, -2, 0);
	emitterData.EmissionRate = 1000;
	emitterData.StartSize = 0.5;
	emitterData.EndSize = 0.5;
	emitterData.NumOfParticles = 1000;
	emitterData.MinParticleLifeTime = 5;
	emitterData.MaxParticleLifeTime = 10;
	const float bounds = 0;
	emitterData.MaxEmissionArea = (CU::Vector3f(bounds, bounds * 2.f, bounds));
	emitterData.MinEmissionArea = (CU::Vector3f(-bounds, 0.f, -bounds));
	const float vel = 10;
	const float nVel = vel / 10;
	emitterData.MinEmissionVelocity = CU::Vector3f(-nVel, vel / 2.f,-nVel);
	emitterData.MaxEmissionVelocity = CU::Vector3f(nVel, vel, nVel);
	emitterData.TexturePath = "Models/Textures/T_M_Rock_10m_RMA.dds";
	emitterData.StartColor = CU::Vector4f(0, 1, 1, 1);
	emitterData.EndColor = CU::Vector4f(1, 1, 1, 1);
	CParticleEmitterComponent* companent = CParticleEmitterComponentManager::GetInstance().CreateComponent(emitterData);
	return companent->GetId();
}
