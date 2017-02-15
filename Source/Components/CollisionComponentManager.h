#pragma once

#include "../PostMaster/Subscriber.h"

namespace CU
{
	class Time;
}

class CCollisionComponent;
class CCollisionManager;
class ICollider;

namespace Intersection
{
	union CollisionData;
}

class CCollisionComponentManager : public Subscriber
{
public:
	enum class eColliderType
	{
		eCircle,
		ePoint,
		eSquare
	};

	CCollisionComponentManager();
	~CCollisionComponentManager();

	void Update();
	void Render();

	CCollisionComponent* CreateCollisionComponent(const eColliderType aColliderType, Intersection::CollisionData& aCollisionData);
	void DestroyCollisionComponent(CCollisionComponent* aCollisionComponent);

	void FlipShouldRender();

private:
	ICollider* CreateCollider(const eColliderType aColliderType, Intersection::CollisionData& aCollisionData);
	eMessageReturn Recieve(const Message& aMessage) override;

	CU::GrowingArray<CCollisionComponent*, unsigned int, false> myCollisionComponents;
	CCollisionManager* myCollisionManager;

	bool myShouldRender;
};
