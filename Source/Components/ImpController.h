#pragma once
#include "Enemy.h"

enum class eImpState
{
	eIdle,
	eWalkIntoMeleeRange,
	eUseMeleeAttack,
	eUseRangedAttack,
	eJump,
	eRunAfterShooting,
	eChargingRangedAttack,
	eChargingMeleeAttack,
	eChase,
	eWaitAfterMeleeAttack,
	eDead
};
class CImpController : public CEnemy
{
public:
	CImpController(unsigned int aId, eEnemyTypes aType);
	~CImpController();

	void Update(const float aDeltaTime) override;
	virtual void SetEnemyData(const SEnemyBlueprint* aData) override;
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData) override;

	bool CheckIfInAir();
	void ApplyJumpForce(float aJumpHeight);
	inline void Sthap() override;
private:
	inline bool ShouldJumpAfterPlayer();
	void UpdateJumpForces(const float aDeltaTime);

	void InitiateWander();
	bool CanChangeState();
	eMessageReturn DoEvent(const CResetToCheckPointMessage& aResetToCheckPointMessage) override;
	void ChangeClientAnimation(const eComponentMessageType aMessageType) const;

private:
	CU::Vector3f myWanderToPosition;

	float myJumpHeight;
	float myJumpForce;
	float myWanderDistance;
	float myWanderDuration;
	float myElaspedWanderTime;
	float myChargeRangedAttackDuration;
	float myElapsedChargeAttackTime;
	float myChargeMeleeAttackDuration;
	float myElapsedChargeMeleeAttackTime;
	float myWaitAfterAttackCountDown;
	eImpState myState;

	bool myIsJumping;
	bool myShouldntExist;
	unsigned char myWanderAngle;
	short myAttacksUntillRunningAway;
	short myUsedAttackSinceLastRunning;
};

inline bool CImpController::ShouldJumpAfterPlayer()
{
	return (myToPlayer.y > 2.0f) && (myIsJumping == false);
}

inline void CImpController::Sthap()
{
	myShouldntExist = true;
}