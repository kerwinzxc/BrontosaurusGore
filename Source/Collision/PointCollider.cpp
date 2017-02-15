#include "stdafx.h"
#include "PointCollider.h"

Intersection::SPoint CPointCollider::ourNullPoint;

CPointCollider::CPointCollider()
{
	myPointData = new Intersection::SPoint();
}

CPointCollider::CPointCollider(const CU::Vector2f aPosition)
{
	myPointData = new Intersection::SPoint();
	myPointData->myPosition = aPosition;
}

CPointCollider::CPointCollider(const CPointCollider& aCopy)
{
	myPointData = new Intersection::SPoint(*aCopy.myPointData);
}

CPointCollider::~CPointCollider()
{
	SAFE_DELETE(myPointData);
}

void CPointCollider::RenderDebugLines(CCollisionRenderer& aCollisionRenderer)
{
	Collision::CRenderPoint* renderPointMessage = new Collision::CRenderPoint(GetData());
	aCollisionRenderer.AddToRender(renderPointMessage);
}

bool CPointCollider::TestCollision(ICollider* aCollider)
{
	return aCollider->TestCollision(this);
}

bool CPointCollider::TestCollision(CPointCollider* aPointCollider)
{
	// A point cannot collide with another point due to floating point presision
	// as long as it is not the same point and then they will (should) not be tested anyway
	return false;
}

bool CPointCollider::TestCollision(CCircleCollider* aCircleCollider)
{
	return Intersection::PointInsideCircle(aCircleCollider->GetData(), GetData());
}

bool CPointCollider::TestCollision(CSquareCollider* aSquareCollider)
{
	return Intersection::PointInsideSquare(aSquareCollider->GetData(), GetData());
}

bool CPointCollider::TestCollision(CTriangleCollider* aTriangleCollider)
{
	return Intersection::PointInsideTriangle(aTriangleCollider->GetData(), GetData());
}

bool CPointCollider::TestCollision(CGroupCollider* aGroupCollider)
{
	return aGroupCollider->TestCollision(this);
}

const Intersection::SPoint& CPointCollider::GetData() const
{
	if (myPointData == nullptr)
	{
		assert(!"Point data in point collider is NULL");
		return ourNullPoint;
	}

	return *myPointData;
}

void CPointCollider::SetPosition(const CU::Vector3f& aPosition)
{
	SetPosition({ aPosition.x, aPosition.z });
}

void CPointCollider::SetPosition(const CU::Vector2f aPosition)
{
	if (myPointData != nullptr)
	{
		myPointData->myPosition = aPosition;
	}
}
