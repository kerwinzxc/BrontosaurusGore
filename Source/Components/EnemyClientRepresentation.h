#pragma once
#include "Component.h"

class CEnemyClientRepresentation : public CComponent
{
public:
	CEnemyClientRepresentation(unsigned int anId);
	~CEnemyClientRepresentation();

	void SetFutureMatrix(const CU::Matrix44f& aMatrix);
	void Update(float aDeltaTime);
	void Init();
protected:
	unsigned myComponentId;
	CU::Matrix44f myFutureMatrix;
	float myPositionInterpolationSpeed;
	float myRotationInterpolationSpeed;
};

