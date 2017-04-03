#pragma once
#include "HealthPoint.h"
struct SEnemyBlueprint
{
	float speed;
	float detectionRange;
	float startAttackRange;
	float stopAttackRange;
	float walkToMeleeRange;
};

struct SImpBlueprint : public SEnemyBlueprint
{
	float jumpHeight;
	float wanderDistance;
	float wanderDuration;
	float chargeAttackDuration;
	float chargeMeleeAttackDuration;
	unsigned char wanderAngle;
	char attacksUntillRunningAway;
};

struct SRevenantBlueprint : public SEnemyBlueprint
{
	float flightHeight;
	float hoverTime;
	float chargeMeleeAttackDuration;
	float chargeRangedAttackAttackDuration;
	float chargeAirBarrageAttackDuration;
};

struct SPinkyBlueprint : public SEnemyBlueprint
{
	float windupChargeTime;
	float chargeSpeed;
	float chargeCooldown;
	float chargeDistance;
	float chargeMeleeAttackDuration;
	healthPoint chargeDamage;
};