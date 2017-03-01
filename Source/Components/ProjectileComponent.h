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
	void Deactivate();
	void Update(float aDeltaTime);
	inline bool GetIsActive();
private:
	SProjectileData* myData;
	CU::Vector3f myDirection;
	CU::Vector3f myStartPosition;
	bool myIsActive;
};

inline bool CProjectileComponent::GetIsActive()
{
	return myIsActive;
}