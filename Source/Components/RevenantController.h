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
private:
	void ApplyFlightForce();
	void UpdateFlightForces(const float aDeltaTime);
	bool CheckIfInAir();
	bool CanChangeState();
private:
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
	eRevenantState myState;

	bool myIsflying;
};

