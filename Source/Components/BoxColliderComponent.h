#pragma once
#include "ColliderComponent.h"


struct SBoxColliderData : public SColliderData
{
	SBoxColliderData() { myType = eColliderType::eBox; }
	CU::Vector3f myHalfExtent;
};

class CBoxColliderComponent : public CColliderComponent
{
public:
	CBoxColliderComponent(SBoxColliderData* aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor);
	~CBoxColliderComponent();


	const SBoxColliderData* GetData() override { return &myData; };
	inline SColliderData::eColliderType GetType() { return SColliderData::eColliderType::eBox; };

private:
	SBoxColliderData myData;
};

