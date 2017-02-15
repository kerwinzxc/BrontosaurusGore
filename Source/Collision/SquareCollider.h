#pragma once
#include "ICollider.h"

namespace Intersection
{
	struct SSquare;
}

class CSquareCollider : public ICollider
{
public:
	CSquareCollider();
	CSquareCollider(const CSquareCollider& aCopy);
	CSquareCollider(CSquareCollider&& aTemporary);
	~CSquareCollider();

	CSquareCollider& operator=(const CSquareCollider& aCopy);
	CSquareCollider& operator=(CSquareCollider&& aTemporary);

	bool TestCollision(ICollider* aCollider) override;
	bool TestCollision(CPointCollider* aPointCollider) override;
	bool TestCollision(CCircleCollider* aCircleCollider) override;
	bool TestCollision(CSquareCollider* aSquareCollider) override;
	bool TestCollision(CTriangleCollider* aTriangleCollider) override;
	bool TestCollision(CGroupCollider* aGroupCollider) override;

	void SetPosition(const CU::Vector3f& aPosition) override;
	void SetPosition(const CU::Vector2f aPosition) override;

	const Intersection::SSquare& GetData() const;

private:
	Intersection::SSquare* mySquareData;

	const static Intersection::SSquare ourNullSquare;
};
