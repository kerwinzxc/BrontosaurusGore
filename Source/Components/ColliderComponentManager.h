#pragma once
#include "ColliderComponent.h"

class CColliderComponentManager
{

public:
	CColliderComponentManager();
	~CColliderComponentManager();
	CColliderComponent* CreateComponent(SColliderData* aColliderData);

private:
	CU::GrowingArray<CColliderComponent*> myColliderComponents;
};
