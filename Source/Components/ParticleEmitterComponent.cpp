#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "GameObject.h"
#include "ComponentManager.h"
#include "ParticleEmitterComponentManager.h"
#include "../BrontosaurusEngine/ParticleEmitterInstance.h"
#include "ParticleEmitterManager.h"

CParticleEmitterComponent::CParticleEmitterComponent(int anId)
{
	myInstanceId = anId;
	myType = eComponentType::eParticleEmitter;
}


CParticleEmitterComponent::~CParticleEmitterComponent()
{
	CParticleEmitterManager::GetInstance().Release(myInstanceId);
}

void CParticleEmitterComponent::Update(CU::Time aDeltaTime)
{
	if (!GetParent())
	{
		return;
	}
	//myParticleInstance->SetPosition(GetParent()->GetToWorldTransform().GetPosition());
	CParticleEmitterManager::GetInstance().SetTransformation(myInstanceId, GetParent()->GetToWorldTransform());
	//myParticleInstance->Update(aDeltaTime);
}
void CParticleEmitterComponent::Activate()
{
	CParticleEmitterManager::GetInstance().Activate(myInstanceId);
}

void CParticleEmitterComponent::Deactivate()
{
	CParticleEmitterManager::GetInstance().Deactivate(myInstanceId);
}


void CParticleEmitterComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eActivateEmitter:
		Activate();
		break;
	default: break;
	}
}

void CParticleEmitterComponent::Destroy()
{
	CParticleEmitterComponentManager::GetInstance().Remove(this);
}
