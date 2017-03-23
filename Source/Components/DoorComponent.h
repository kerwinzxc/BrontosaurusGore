#pragma once
#include "Component.h"

typedef short lockID;

class CDoorComponent : public CComponent
{
public:
	CDoorComponent();
	~CDoorComponent();

	void SetOpenDirection(const CU::Vector2f& aDirection);
	void SetMoveSpeed(const float aMoveSpeed);
	void SetMoveDistance(const float aDistance);
	void SetIsClosed(const bool aIsClosed);
	void SetIsLocked(const bool aIsLocked);
	void SetLockId(const lockID aLockId);

	void SetNetworkID(const unsigned char aNetworkID);

	void Update(const CU::Time& aDeltaTime);

	const CU::Vector2f GetOpenDirection() const;
	const float GetMoveSpeed() const;
	const float GetMoveDistance() const;
	const lockID GetLockId() const;

	const bool GetIsClosed() const;
	const bool GetIsLocked() const;
	const unsigned char GetNetworkID() const;


	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	CU::Vector2f myOriginPosition;
	CU::Vector2f myOpenDirection;

	float myMoveSpeed;
	float myMoveDistance;

	lockID myLockId;

	bool myIsClosed;
	bool myIsLocked;

	unsigned char myNetworkID;
};

