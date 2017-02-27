#pragma once
#include "ColliderComponent.h"


struct SBoxColliderData : public SColliderData
{
	SBoxColliderData() { myType = eColliderType::eBox; }
	CU::Vector3f myCenter;
	CU::Vector3f mySize;
};

class CBoxColliderComponent : public CColliderComponent
{
public:
	CBoxColliderComponent(SBoxColliderData* aColliderData);
	~CBoxColliderComponent();

private:
	SBoxColliderData myData;
};

