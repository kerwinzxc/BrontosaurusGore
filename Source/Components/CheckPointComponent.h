#pragma once
#include "Component.h"
class CCheckPointComponent : public CComponent
{
public:
	CCheckPointComponent();
	~CCheckPointComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
	void Update(float aDeltaTime);
	void SetAsNewCheckPoint();
	inline void SetCheckPointPosition(CU::Vector3f aPositionToRespawnAt);
private:
	CU::Vector3f myRespawnPosition;
	bool myHaveBeenActivated;

};

inline void CCheckPointComponent::SetCheckPointPosition(CU::Vector3f aPositionToRespawnAt)
{
	myRespawnPosition = aPositionToRespawnAt;
}