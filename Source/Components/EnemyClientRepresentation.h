#pragma once
#include "Component.h"
#include "EnemyTypes.h"
#include "Enemy.h"

class CEnemyClientRepresentation : public CEnemy
{
public:
	CEnemyClientRepresentation(unsigned int anId, const eEnemyTypes aType = eEnemyTypes::eDefault);
	~CEnemyClientRepresentation();

	void SetFutureMatrix(const CU::Matrix44f& aMatrix);
	void Update(float aDeltaTime);
	void Init();
	inline const eEnemyTypes GetEnemyType() const;
	inline void SetEnemyType(const eEnemyTypes aType);
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
	myType = aType;
}