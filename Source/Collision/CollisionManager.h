#pragma once

class ICollider;
class CCollisionRenderer;

class CCollisionManager
{
public:
	CCollisionManager();
	~CCollisionManager();

	void Update();
	void Render();

	void AddCollider(ICollider* aCollider);
	void RemoveCollider(ICollider* aCollider);

	const CU::GrowingArray<char, unsigned int, false>& GetLineVertexBuffer();

private:
	inline void TestCollision(ICollider* aFirst, ICollider* aSecond);

	CU::GrowingArray<ICollider*, int, false> myColliders;
	CCollisionRenderer* myCollisionRenderer;
	bool myShouldRenderDebugLines;
};
