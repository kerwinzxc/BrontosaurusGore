#pragma once
#include "Component.h"
#include "PlayerControls.h"
#include "../CommonUtilities/BitSet.h"
#include "../Physics/PhysicsCharacterController.h"
#include "../Audio/AudioInterface.h"

class CTextInstance;

class CMovementComponent : public CComponent
{
public:
	CMovementComponent();
	~CMovementComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	void Update(const CU::Time aDeltaTime);
	void SetIntroFallMode();
private:
	void SwapMovementMode();
	bool IsWalking() const;
	void UpdateSoundState();
	void DefaultMovement(const CU::Time& aTime);
	void FreecamMovement(const CU::Time& aTime);
	void KeyPressed(const ePlayerControls aPlayerControl);
	void KeyReleased(const ePlayerControls aPlayerControl);

	void ApplyJumpForce(float aJumpHeight);
	void ResetIdle();
private:
	enum class MovementMode
	{
		Default,
		Freecam,
		Size
	};

	CU::CBitSet<static_cast<int>(ePlayerControls::eLength)> myKeysDown;

	CTextInstance* myTextInstance;

	CU::Vector3f myVelocity;
	float myAcceleration;
	float myDeceleration;
	float myMaxSpeed;

	float myJumpHeight;
	float myDoubleJumpHeight;
	float myJumpForce;
	float myFrameLastPositionY;
	float myElapsedFallCheckTime;
	float myFallCheckCooldown;
	float myLastGroundedPositionY;

	float myIdleCountdown;
	float myIdleThingCountdown;

	char myControllerConstraints;
	bool myCanDoubleJump;
	bool myIsNotFalling;
	MovementMode myMovementMode;
	bool myNoclipProssed;
	int mySpeedMultiplier;
	bool myIncrementPressed;
	bool myDecrementPressed;
	bool myIsWalking;

	bool myUseConstantVelosity;
	Audio::GameObjectID myAudioId;
};
