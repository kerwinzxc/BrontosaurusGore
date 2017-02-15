
#pragma once
#include "../CommonUtilities/TimerManager.h"
#include "Component.h"
#include "../BrontosaurusEngine/StreakEmitterInstance.h"

class CStreakEmitterComponent : public CComponent
{
public:
	CStreakEmitterComponent();
	~CStreakEmitterComponent();
	void Init(CStreakEmitterInstance* aParticleEmitterInstance);
	void Update(CU::Time aDeltaTime);

	inline CStreakEmitterInstance* GetEmitter();

	inline void Activate();
	inline void Deactivate();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData);
	void Destroy() override;

private:
	CStreakEmitterInstance* myStreakEmitterInstance;
};

void CStreakEmitterComponent::Activate()
{
	myStreakEmitterInstance->Activate();
}

void CStreakEmitterComponent::Deactivate()
{
	myStreakEmitterInstance->Deactivate();
}

CStreakEmitterInstance* CStreakEmitterComponent::GetEmitter()
{
	return myStreakEmitterInstance;
}
