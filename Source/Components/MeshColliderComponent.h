#pragma once
#include "ColliderComponent.h"

struct SMeshColliderData : public SColliderData
{
	SMeshColliderData() { myType = eColliderType::eMesh; }
	const char* myPath;
};

class CMeshColliderComponent : public CColliderComponent
{
public:
	CMeshColliderComponent(const SMeshColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor);
	~CMeshColliderComponent();

	const SMeshColliderData* GetData() override { return &myData; };
	inline SColliderData::eColliderType GetType() { return SColliderData::eColliderType::eMesh; };

private:
	SMeshColliderData myData;
};

