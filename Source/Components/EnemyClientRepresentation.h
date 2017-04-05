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
protected:
	unsigned myComponentId;
	CU::Matrix44f myFutureMatrix;
	float myPositionInterpolationSpeed;
	float myRotationInterpolationSpeed;
	eEnemyTypes myType;
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