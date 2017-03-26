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
class CRevenantController : public CEnemy
{
public:
	CRevenantController(unsigned int aId, eEnemyTypes aType);
	~CRevenantController();
};

