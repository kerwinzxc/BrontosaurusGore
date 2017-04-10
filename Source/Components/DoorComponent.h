#pragma once
#include "Component.h"
#include "../ThreadedPostmaster/Subscriber.h"

typedef short lockID;

class CDoorComponent : public CComponent, public Postmaster::ISubscriber
{
public:
	CDoorComponent();
	~CDoorComponent();

	void SetOpenDirection(const CU::Vector2f& aDirection);
	void SetMoveSpeed(const float aMoveSpeed);
	void SetMoveDistance(const float aDistance);
	void SetIsClosed(const bool aIsClosed);
	void SetShouldReset(const bool aShouldReset);
	void SetIsLocked(const bool aIsLocked);
	void SetLockId(const lockID aLockId);

	void SnapShotDoorState();
	void ResetToSnapShot();

	void SetNetworkID(const unsigned char aNetworkID);

	void Update(const CU::Time& aDeltaTime);

	const CU::Vector2f GetOpenDirection() const;
	const float GetMoveSpeed() const;
	const float GetMoveDistance() const;
	const lockID GetLockId() const;

	const bool GetIsClosed() const;
	const bool GetIsLocked() const;
	const unsigned char GetNetworkID() const;

	
	eMessageReturn DoEvent(const CSetAsNewCheckPointMessage & aSetAsNewCheckPointMessage) override;

	eMessageReturn CDoorComponent::DoEvent(const CResetToCheckPointMessage& aResetToCheckPointMessage) override;

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	CU::Matrix44f myResetToPosition;
	CU::Vector2f myOriginPosition;
	CU::Vector2f myOpenDirection;

	float myMoveSpeed;
	float myMoveDistance;

	lockID myLockId;

	bool myIsClosed;
	bool myIsLocked;
	bool myShouldReset;

	bool myResetToIsClosed;
	bool myResetToIsLocked;

	unsigned char myNetworkID;

};

