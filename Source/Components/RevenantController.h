#pragma once
#include "Enemy.h"
enum class eRevenantState
{
	eIdle,
	eWalkIntoMeleeRange,
	eUseMeleeAttack,
	eUseRangedAttack,
	eFlyAscend,
	eFlyDescend,
	eFlyHover,
	eChargingRangedAttack,
	eChargingMeleeAttack,
	eChase,
	eWaitBeforeChangingState,
	eDead
};
class CRevenantController : public CEnemy
{
public:
	CRevenantController(unsigned int aId, eEnemyTypes aType);
	~CRevenantController();

	virtual void SetEnemyData(const SEnemyBlueprint* aData) override;
	void Update(const float aDeltaTime) override;
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData) override;
	eMessageReturn DoEvent(const CResetToCheckPointMessage& aResetToCheckPointMessage) override;
	inline void Sthap() override;
private:
	void ApplyFlightForce();
	void UpdateFlightForces(const float aDeltaTime);
	bool CheckIfInAir();
	bool CanChangeState();
	void StartCharginRangedAttack();
private:
	CU::Vector3f myJumpPointPosition;

	float myFlightHeight;
	float myHoverTime;
	float myStartedFlightHeight;
	float myFlightForce;
	float myElapsedHoverTime;
	float myChargeRangedAttackDuration;
	float myElapsedChargeRangedAttackTime;
	float myChargeRangedAirBarrageAttackDuration;
	float myElapsedChargeRangedAirBarrageAttackTime;
	float myChargeMeleeAttackDuration;
	float myElapsedChargeMeleeAttackTime;
	float myChillAtJumpPointCountDown;
	float myWaitBeforeChangingStateCountdown;
	float myWaitUntilStopFlyingCountdown;
	unsigned short myAttacksUntilChangingStates;
	unsigned short myUsedAttacksSinceLastStateChange;
	eRevenantState myState;

	bool myIsflying;
	bool myIsAtJumpPoint;
	bool myShouldntExist;
};

inline void CRevenantController::Sthap()
{
	myShouldntExist = true;
}
