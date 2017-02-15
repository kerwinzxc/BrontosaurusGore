#include "stdafx.h"
#include "TriangleCollider.h"

const Intersection::STriangle CTriangleCollider::ourNullTriangle{ CU::Vector2f::Zero };

CTriangleCollider::CTriangleCollider()
{
	myTriangleData = new Intersection::STriangle();
}

CTriangleCollider::CTriangleCollider(Intersection::STriangle* aTriangleData)
{
	myTriangleData = aTriangleData;
}

CTriangleCollider::CTriangleCollider(const CU::StaticArray<CU::Vector2f, 3>& aPoints)
	: CTriangleCollider()
{
	myTriangleData->myPoints = aPoints;
}

CTriangleCollider::CTriangleCollider(const CTriangleCollider& aCopy)
{
	self = aCopy;
}

CTriangleCollider::CTriangleCollider(CTriangleCollider&& aTemporary)
{
	self = std::move(aTemporary);
}

CTriangleCollider::~CTriangleCollider()
{
	SAFE_DELETE(myTriangleData);
}

CTriangleCollider& CTriangleCollider::operator=(const CTriangleCollider& aCopy)
{
	if (myTriangleData == nullptr)
	{
		myTriangleData = new Intersection::STriangle();
	}

	myTriangleData->myPoints = aCopy.myTriangleData->myPoints;

	return self;
}

CTriangleCollider& CTriangleCollider::operator=(CTriangleCollider&& aTemporary)
{
	myTriangleData = aTemporary.myTriangleData;
	aTemporary.myTriangleData = nullptr;

	return self;
}

void CTriangleCollider::RenderDebugLines(CCollisionRenderer& aCollisionRenderer)
{
	Collision::CRenderTriangle* renderTriangleCommand = new Collision::CRenderTriangle(&GetData());
	aCollisionRenderer.AddToRender(renderTriangleCommand);
}

bool CTriangleCollider::TestCollision(ICollider* aCollider)
{
	return aCollider->TestCollision(this);
}

bool CTriangleCollider::TestCollision(CPointCollider* aPointCollider)
{
	return Intersection::PointInsideTriangle(GetData(), aPointCollider->GetData());
}

bool CTriangleCollider::TestCollision(CCircleCollider* aCircleCollider)
{
	return Intersection::CircleVsTriangle(aCircleCollider->GetData(), GetData());
}

bool CTriangleCollider::TestCollision(CSquareCollider* aSquareCollider)
{
	return Intersection::SquareVsTriangle(aSquareCollider->GetData(), GetData());
}

bool CTriangleCollider::TestCollision(CGroupCollider* aGroupCollider)
{
	return aGroupCollider->TestCollision(this);
}

void CTriangleCollider::SetPosition(const CU::Vector3f& aPosition)
{
	SetPosition({ aPosition.x, aPosition.y });
}

void CTriangleCollider::SetPosition(const CU::Vector2f aPosition)
{
	if (myTriangleData == nullptr)
	{
		return;
	}

	myTriangleData->myPoints[0] = aPosition;
}

const Intersection::STriangle& CTriangleCollider::GetData() const
{
	if (myTriangleData == nullptr)
	{
		return ourNullTriangle;
	}

	return *myTriangleData;
}
