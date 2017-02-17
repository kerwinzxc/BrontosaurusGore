#pragma once
#include "Component.h"
#include "PlayerControls.h"
#include "../CommonUtilities/BitSet.h"

class CMovementComponent : public CComponent
{
public:
	CMovementComponent();
	~CMovementComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Update(const CU::Time aDeltaTime);

private:
	void KeyPressed(const ePlayerControls aPlayerControl);
	void KeyReleased(const ePlayerControls aPlayerControl);

private:
	CU::CBitSet<static_cast<int>(ePlayerControls::eLength)> myKeysDown;

	CU::Vector3f myVelocity;
	float myAcceleration;
	float myDeceleration;
	float myMaxSpeed;
};