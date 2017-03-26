#pragma once
#include "Enemy.h"
#include "HealthPoint.h"
enum class ePinkyState
{
	eIdle,
	eWindupCharge,
	eCharge,
	eChargeCooldown,
	eUseMeleeAttack,
	eWalkIntoMeleeRange,
	eDead
};
class CPinkyController : public CEnemy
{
public:
	CPinkyController(unsigned int aId, eEnemyTypes aType);
	~CPinkyController();

	virtual void SetEnemyData(const SEnemyBlueprint* aData) override;
	void Update(const float aDeltaTime) override;
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData);
private:
	float myWindupChargeTime;
	float myElapsedWindupTime;
	float myChargeSpeed;
	float myGravityForce;
	float myChargeCooldown;
	float myElapsedChargeCooldownTime;
	ePinkyState myState;
	bool myIsCharging;
	healthPoint myChargeDamage;
};

