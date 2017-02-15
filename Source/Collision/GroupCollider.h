#pragma once
#include "ICollider.h"

class CGroupCollider : public ICollider
{
public:
	using size_type = unsigned short;

	CGroupCollider();
	CGroupCollider(const CGroupCollider& aCopy);
	CGroupCollider(CGroupCollider&& aTemporary);
	~CGroupCollider();

	CGroupCollider& operator=(const CGroupCollider& aCopy);
	CGroupCollider& operator=(CGroupCollider&& aTemporary);

	void RenderDebugLines(CCollisionRenderer& aCollisionRenderer) override;

	bool TestCollision(ICollider* aCollider) override;
	bool TestCollision(CPointCollider* aPointCollider) override;
	bool TestCollision(CCircleCollider* aCircleCollider) override;
	bool TestCollision(CSquareCollider* aBoxCollider) override;
	bool TestCollision(CGroupCollider* aGroupCollider) override;

	void SetPosition(const CU::Vector3f& aPosition) override;
	void SetPosition(const CU::Vector2f aPosition) override;

private:
	CU::GrowingArray<ICollider*, size_type, false> myColliders;
};
