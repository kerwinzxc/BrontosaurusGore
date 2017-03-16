#pragma once
#include "ColliderComponent.h"

struct SSphereColliderData : public SColliderData
{
	SSphereColliderData() { myType = eColliderType::eSphere; }
	float myRadius;
};

class CSphereColliderComponent : public CColliderComponent
{
public:
	CSphereColliderComponent(const SSphereColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor);
	~CSphereColliderComponent();

	const SSphereColliderData* GetData() override { return &myData; };
	inline SColliderData::eColliderType GetType() { return SColliderData::eColliderType::eSphere; };
private:
	SSphereColliderData myData;
};

