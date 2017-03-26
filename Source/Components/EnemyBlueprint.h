#pragma once
#include "HealthPoint.h"
struct SEnemyBlueprint
{
	float speed;
	float detectionRange;
	float startAttackRange;
	float stopAttackRange;
	float shouldGoMeleeRadius;
};

struct SImpBlueprint : public SEnemyBlueprint
{
	float jumpHeight;
};

struct SRevenantBlueprint : public SEnemyBlueprint
{
	float flightHeight;
	float flyingTime;
};

struct SPinkyBlueprint : public SEnemyBlueprint
{
	float windupChargeTime;
	healthPoint chargeDamage;
};