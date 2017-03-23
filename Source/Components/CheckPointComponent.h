#pragma once
#include "Component.h"
class CCheckPointComponent : public CComponent
{
public:
	CCheckPointComponent(const unsigned char aId);
	~CCheckPointComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
	void Update(float aDeltaTime);
	void SetAsNewCheckPoint();
	void SetAsNewCheckPointWithNetwork();
	inline void SetCheckPointPosition(CU::Vector3f aPositionToRespawnAt);
private:
	CU::Vector3f myRespawnPosition;
	const unsigned char myId;
	bool myHaveBeenActivated;

};

inline void CCheckPointComponent::SetCheckPointPosition(CU::Vector3f aPositionToRespawnAt)
{
	myRespawnPosition = aPositionToRespawnAt;
}