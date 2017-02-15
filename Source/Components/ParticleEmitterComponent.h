#pragma once
#include "../CommonUtilities/TimerManager.h"
#include "Component.h"
#include "../BrontosaurusEngine/ParticleEmitterInstance.h"

class CParticleEmitterComponent : public CComponent
{
public:
	CParticleEmitterComponent(CParticleEmitterInstance * aEmitterInstance);
	~CParticleEmitterComponent();
	void Update(CU::Time aDeltaTime);

	inline CParticleEmitterInstance* GetEmitter();

	inline void Activate();
	inline void Deactivate();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData);

	void Destroy() override;

private:
	//Keep in Scene and have pointer to scene one?
	CParticleEmitterInstance* myParticleInstance;
};

void CParticleEmitterComponent::Activate()
{
	myParticleInstance->Activate();
}

void CParticleEmitterComponent::Deactivate()
{
	myParticleInstance->Deactivate();
}

CParticleEmitterInstance* CParticleEmitterComponent::GetEmitter()
{
	return myParticleInstance;
}