#pragma once
#include "ICollider.h"

namespace CU
{
	template<typename ObjectType, int ObjectCapacity>
	class CStaticMemoryPool;
}

namespace Intersection
{
	struct SCircle;
}

class CCircleCollider :	public ICollider
{
public:
	CCircleCollider();
	CCircleCollider(Intersection::SCircle* someCircleData);
	CCircleCollider(CU::Vector3f aPosition, float aRadius);
	CCircleCollider(const CCircleCollider& aCopy);
	CCircleCollider(CCircleCollider&& aTemporary);
	~CCircleCollider();

	CCircleCollider& operator=(const CCircleCollider& aCopy);
	CCircleCollider& operator=(CCircleCollider&& aTemporary);

	void* operator new(size_t aBytes);
	void operator delete(void* aObject);
	
	void RenderDebugLines(CCollisionRenderer& aCollisionRenderer) override;

	bool TestCollision(ICollider* aCollider) override;
	bool TestCollision(CPointCollider* aCollider) override;
	bool TestCollision(CCircleCollider* aCircleCollider) override;
	bool TestCollision(CSquareCollider* aBoxCollider) override;
	bool TestCollision(CTriangleCollider* aTriangleCollider) override;
	bool TestCollision(CGroupCollider* aGroupCollider) override;

	void SetPosition(const CU::Vector3f& aPosition) override;
	void SetPosition(const CU::Vector2f aPosition) override;
	
	const Intersection::SCircle& GetData() const;

private:
	Intersection::SCircle* myCircleData;

	const static Intersection::SCircle ourNullCircle;
	static CU::CStaticMemoryPool<CCircleCollider, 2048> ourMemoryPool;
};
