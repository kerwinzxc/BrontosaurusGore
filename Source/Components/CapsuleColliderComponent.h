#pragma once
#include "ColliderComponent.h"

struct SCapsuleColliderData : public SColliderData
{
	SCapsuleColliderData() { myType = eColliderType::eCapsule; }
	float myHeight;
	float myRadius;
};

class CCapsuleColliderComponent : public CColliderComponent
{
public:
	CCapsuleColliderComponent(const SCapsuleColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor);
	~CCapsuleColliderComponent();


	const SCapsuleColliderData* GetData() override { return &myData; };
	inline SColliderData::eColliderType GetType() { return SColliderData::eColliderType::eBox; };

private:
	SCapsuleColliderData myData;
};

