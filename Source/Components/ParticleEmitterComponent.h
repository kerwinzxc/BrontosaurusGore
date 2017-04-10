#pragma once
#include "../CommonUtilities/TimerManager.h"
#include "Component.h"

class CParticleEmitterInstance;

class CParticleEmitterComponent : public CComponent
{
public:
	CParticleEmitterComponent(CParticleEmitterInstance * aEmitterInstance);
	~CParticleEmitterComponent();
	void Update(CU::Time aDeltaTime);

	inline CParticleEmitterInstance* GetEmitter();

	void Activate();
	void Deactivate();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData);

	void Destroy() override;

private:
	//Keep in Scene and have pointer to scene one?
	CParticleEmitterInstance* myParticleInstance;
};



CParticleEmitterInstance* CParticleEmitterComponent::GetEmitter()
{
	return myParticleInstance;
}