#pragma once
#include "Component.h"
struct SExplosionData;
class CExplosionComponent : public CComponent
{
public:
	CExplosionComponent();
	~CExplosionComponent();
	void Update(float aDeltaTime);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
private:
	CU::GrowingArray<CGameObject*> myCollidedWithGameObjects;
	SExplosionData* myData;
	float myDuration;
	float myElapsedLivingTime;
	bool myIsActive;
};

