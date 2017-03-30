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
private:
	inline bool ShouldJumpAfterPlayer();
	void UpdateJumpForces(const float aDeltaTime);

	void InitiateWander();
private:
	CU::Vector3f myWanderToPosition;

	float myJumpHeight;
	float myJumpForce;
	float myWanderDistance;
	float myWanderDuration;
	float myElaspedWanderTime;
	eImpState myState;

	bool myIsJumping;
	unsigned char myWanderAngle;
	char myAttacksUntillRunningAway;
	char myUsedAttackSinceLastRunning;
};

inline bool CImpController::ShouldJumpAfterPlayer()
{
	return (myToPlayer.y > 1.0f) && (myIsJumping == false);
}
