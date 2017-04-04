#pragma once
#include "HealthPoint.h"
struct SEnemyBlueprint
{
	float speed;
	float detectionRange;
	float startAttackRange;
	float stopAttackRange;
	float walkToMeleeRange;
	float chargeMeleeAttackDuration;
	healthPoint Health;
};

struct SImpBlueprint : public SEnemyBlueprint
{
	float jumpHeight;
	float wanderDistance;
	float wanderDuration;
	float chargeAttackDuration;
	unsigned char wanderAngle;
	char attacksUntillRunningAway;
};

struct SRevenantBlueprint : public SEnemyBlueprint
{
	float flightHeight;
	float hoverTime;
	float chargeRangedAttackAttackDuration;
	float chargeAirBarrageAttackDuration;
};

struct SPinkyBlueprint : public SEnemyBlueprint
{
	float windupChargeTime;
	float chargeSpeed;
	float chargeCooldown;
	float chargeDistance;
	healthPoint chargeDamage;
};