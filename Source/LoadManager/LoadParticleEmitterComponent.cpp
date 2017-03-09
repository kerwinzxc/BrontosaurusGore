#include "stdafx.h"
#include "LoadParticleEmitterComponent.h"
#include "ParticleEmitterComponentManager.h"

int LoadParticleEmitterComponent(KLoader::SLoadedComponentData someData)
{
	SEmitterData emitterData;
	emitterData.UseGravity = false;
	emitterData.EmissionRate = 10;
	emitterData.StartSize = 0.1;
	emitterData.EndSize = 0.5;
	emitterData.NumOfParticles = 10000;
	emitterData.MinParticleLifeTime = 10;
	emitterData.MaxParticleLifeTime = 20;
	emitterData.MinEmissionVelocity = CU::Vector3f(-1, -1, -1);
	emitterData.MaxEmissionVelocity = CU::Vector3f(1, 1, 1);
	emitterData.TexturePath = "Models/Textures/T_M_Rock_10m_RMA.dds";
	emitterData.StartColor = CU::Vector4f(1, 1, 1, 1);
	emitterData.EndColor = CU::Vector4f(1, 1, 1, 1);
	CParticleEmitterComponent* companent = CParticleEmitterComponentManager::GetInstance().CreateComponent(emitterData);
	return companent->GetId();
}
