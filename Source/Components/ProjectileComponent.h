#pragma once
#include "Component.h"

struct SProjectileData;
class CProjectileComponent : public CComponent
{
public:
	CProjectileComponent();
	~CProjectileComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;

	void Activate(SProjectileData* someData, const CU::Vector3f& aDirection, const CU::Vector3f& aPosition);
	void Update(float aDeltaTime);
private:
	SProjectileData* myData;
	CU::Vector3f myDirection;
	bool myIsActive;
};

