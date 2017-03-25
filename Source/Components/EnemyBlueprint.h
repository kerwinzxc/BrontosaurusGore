#pragma once

struct SEnemyBlueprint
{
	float speed;
	float detectionRange;
	float startAttackRange;
	float stopAttackRange;
};

struct SImpBlueprint : public SEnemyBlueprint
{
	float goMeleeRadius;
	float jumpHeight;

};