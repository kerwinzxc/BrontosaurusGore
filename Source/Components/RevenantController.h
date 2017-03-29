#pragma once
#include "Enemy.h"
enum class eRevenantState
{
	eIdle,
	eWalkIntoMeleeRange,
	eUseMeleeAttack,
	eUseRangedAttack,
	eFlyAscend,
	eFlyDescend,
	eFlyHover,
	eDead
};
class CRevenantController : public CEnemy
{
public:
	CRevenantController(unsigned int aId, eEnemyTypes aType);
	~CRevenantController();

	virtual void SetEnemyData(const SEnemyBlueprint* aData) override;
	void Update(const float aDeltaTime) override;
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData);

private:
	void ApplyFlightForce();
	void UpdateFlightForces(const float aDeltaTime);
	bool CheckIfInAir();
private:
	float myFlightHeight;
	float myHoverTime;
	float myStartedFlightHeight;
	float myFlightForce;
	float myElapsedHoverTime;
	eRevenantState myState;

	bool myIsflying;
};

