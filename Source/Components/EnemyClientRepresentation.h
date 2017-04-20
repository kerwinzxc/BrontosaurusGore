#pragma once
#include "Component.h"
#include "EnemyTypes.h"
#include "Enemy.h"

class CEnemyClientRepresentation : public CEnemy
{
public:
	CEnemyClientRepresentation(unsigned int anId, const eEnemyTypes aType);
	~CEnemyClientRepresentation();

	void SetFutureMatrix(const CU::Matrix44f& aMatrix);
	void Update(float aDeltaTime);
	void Init();
	inline const eEnemyTypes GetEnemyType() const;
	inline void SetEnemyType(const eEnemyTypes aType);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
	void CheckIfOutOfBounds();
protected:
	void DoDeathEffect() override;
	unsigned myComponentId;
	CU::Matrix44f myFutureMatrix;
	CU::Vector3f myShootDirection;
	float myPositionInterpolationSpeed;
	float myRotationInterpolationSpeed;
	float myShootChargeCountDown;
	short myAmountOfCuedShots;
	eEnemyTypes myType;
	bool myIsAlive;
};

inline const eEnemyTypes CEnemyClientRepresentation::GetEnemyType() const
{
	return myType;
}
inline void CEnemyClientRepresentation::SetEnemyType(const eEnemyTypes aType)
{
	SetType(aType);
	myType = aType;
}