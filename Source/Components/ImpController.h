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
class CImpController : public CEnemy
{
public:
	CImpController(unsigned int aId, eEnemyTypes aType);
	~CImpController();

	void Update(const float aDeltaTime) override;
	virtual void SetEnemyData(const SEnemyBlueprint* aData);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData);
private:
	float myJumpHeight;
	float myShouldGoMeleeRadius2;
	eImpState myState;

};

