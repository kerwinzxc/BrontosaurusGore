#pragma once
#include "ICollider.h"

namespace Intersection
{
	struct STriangle;
}

class CTriangleCollider : public ICollider
{
public:
	CTriangleCollider();
	CTriangleCollider(Intersection::STriangle* aTriangleData);
	CTriangleCollider(const CU::StaticArray<CU::Vector2f, 3>& aPoints);
	CTriangleCollider(const CTriangleCollider& aCopy);
	CTriangleCollider(CTriangleCollider&& aTemporary);
	~CTriangleCollider();

	CTriangleCollider& operator=(const CTriangleCollider& aCopy);
	CTriangleCollider& operator=(CTriangleCollider&& aTemporary);

	void RenderDebugLines(CCollisionRenderer& aCollisionRenderer) override;

	bool TestCollision(ICollider* aCollider) override;
	bool TestCollision(CPointCollider* aPointCollider) override;
	bool TestCollision(CCircleCollider* aCircleCollider) override;
	bool TestCollision(CSquareCollider* aSquareCollider) override;
	bool TestCollision(CGroupCollider* aGroupCollider) override;

	void SetPosition(const CU::Vector3f& aPosition) override;
	void SetPosition(const CU::Vector2f aPosition) override;

	const Intersection::STriangle& GetData() const;

private:
	Intersection::STriangle* myTriangleData;

	const static Intersection::STriangle ourNullTriangle;
};
