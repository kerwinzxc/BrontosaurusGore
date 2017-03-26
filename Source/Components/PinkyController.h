#pragma once
#include "Enemy.h"
enum class eImpState
{
	eIdle,
	eWalkIntoMeleeRange,
	eUseMeleeAttack,
	eUseRangedAttack,
	eJump,
	eDead
};
class CPinkyController : public CEnemy
{
public:
	CPinkyController(unsigned int aId, eEnemyTypes aType);
	~CPinkyController();
};

