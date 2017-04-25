#pragma once
#include "Component.h"
class CCheckPointComponent : public CComponent
{
public:
	CCheckPointComponent(const unsigned char aId, const short aIndex);
	~CCheckPointComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
	void Update(float aDeltaTime);
	void SetAsNewCheckPoint();
	void SetAsNewCheckPointWithNetwork();
	inline void SetCheckPointPosition(CU::Vector3f aPositionToRespawnAt);
	inline const short GetIndex() const;
	inline const bool GetHaveBeenActivated() const;
private:
	CU::Vector3f myRespawnPosition;
	short myIndex;
	const unsigned char myId;
	bool myHaveBeenActivated;

};

inline void CCheckPointComponent::SetCheckPointPosition(CU::Vector3f aPositionToRespawnAt)
{
	myRespawnPosition = aPositionToRespawnAt;
}

inline const short CCheckPointComponent::GetIndex() const
{
	return myIndex;
}


inline const bool CCheckPointComponent::GetHaveBeenActivated() const
{
	return myHaveBeenActivated;
}