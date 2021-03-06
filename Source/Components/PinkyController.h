#pragma once
#include "Enemy.h"
#include "HealthPoint.h"
enum class ePinkyState
{
	eIdle,
	eWindupCharge,
	eStartCharge,
	eCharge,
	eChargeCooldown,
	eUseMeleeAttack,
	eWalkIntoMeleeRange,
	eChargingMeleeAttack,
	eChase,
	eDead
};
class CPinkyController : public CEnemy
{
public:
	CPinkyController(unsigned int aId, eEnemyTypes aType);
	~CPinkyController();

	virtual void SetEnemyData(const SEnemyBlueprint* aData) override;
	void Update(const float aDeltaTime) override;
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData) override;
	inline void Sthap() override;

private:
	void HandleGrounded();
	void UpdateChargeCooldown(const float aDeltaTime);
	void KeepWithinChargeDist();
	bool CanChangeState();

private:
	float myWindupChargeTime;
	float myElapsedWindupTime;
	float myChargeSpeed;
	float myGravityForce;
	float myChargeCooldown;
	float myElapsedChargeCooldownTime;
	float myChargeDistance2;
	float myMeleeAttackChargeDuration;
	float myElapsedChargeMeleeAttackTime;
	ePinkyState myState;
	bool myIsCharging;
	bool mySHouldntExist;
	healthPoint myChargeDamage;
	CU::Vector3f myStartChargeLocation;
	CU::Vector3f myLastFramePostion;
};

inline void CPinkyController::Sthap()
{
	mySHouldntExist = true;
}