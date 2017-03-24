#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "GameObject.h"
#include "ComponentManager.h"
#include "ParticleEmitterComponentManager.h"

CParticleEmitterComponent::CParticleEmitterComponent(CParticleEmitterInstance* aEmitterInstance)
{
	myParticleInstance = aEmitterInstance;
	myType = eComponentType::eParticleEmitter;
}


CParticleEmitterComponent::~CParticleEmitterComponent()
{
}

void CParticleEmitterComponent::Update(CU::Time aDeltaTime)
{
	if (!GetParent())
	{
		return;
	}
	myParticleInstance->SetPosition(GetParent()->GetToWorldTransform().GetPosition());
	myParticleInstance->Update(aDeltaTime);
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
