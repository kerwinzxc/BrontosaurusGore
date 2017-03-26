#pragma once
#include "Enemy.h"
#include "HealthPoint.h"
enum class ePinkyState
{
	eIdle,
	eWindupCharge,
	eCharge,
	eTurnTowardsPlayer,
	eDoMeleeAttack,
	eWalkToPlayer,
	eDead
};
class CPinkyController : public CEnemy
{
public:
	CPinkyController(unsigned int aId, eEnemyTypes aType);
	~CPinkyController();

	virtual void SetEnemyData(const SEnemyBlueprint* aData) override;
private:
	CU::Vector3f myChargeTowardsPosition;
	float myWindupChargeTime;
	healthPoint myChargeDamage;
};

