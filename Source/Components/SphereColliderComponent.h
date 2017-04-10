#pragma once
#include "ColliderComponent.h"

struct SSphereColliderData : public SColliderData
{
	SSphereColliderData() { myType = eColliderType::eSphere; }
	float myRadius = 1.0f;
};

class CSphereColliderComponent : public CColliderComponent
{
public:
	CSphereColliderComponent(const SSphereColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor, const bool aIsTrigger);
	~CSphereColliderComponent();

	const SSphereColliderData* GetData() override { return &myData; };
	inline SColliderData::eColliderType GetType() { return SColliderData::eColliderType::eSphere; };
private:
	SSphereColliderData myData;
};

