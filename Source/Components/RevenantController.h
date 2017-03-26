#pragma once
#include "Enemy.h"
enum class eRevenantState
{
	eIdle,
	eWalkIntoMeleeRange,
	eUseMeleeAttack,
	eUseRangedAttack,
	eFlyAscend,
	eFlyHover,
	eDead
};
class CRevenantController : public CEnemy
{
public:
	CRevenantController(unsigned int aId, eEnemyTypes aType);
	~CRevenantController();

	virtual void SetEnemyData(const SEnemyBlueprint* aData) override;

private:
	float myFlightHeight;
	float myFlyingTime;
	float myStartedFlightHeight;
};

