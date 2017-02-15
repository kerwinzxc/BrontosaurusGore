#include "stdafx.h"
#include "SquareCollider.h"

const Intersection::SSquare CSquareCollider::ourNullSquare;

CSquareCollider::CSquareCollider()
{
	mySquareData = new Intersection::SSquare();
}

CSquareCollider::CSquareCollider(const CSquareCollider& aCopy)
{
	self = aCopy;
}

CSquareCollider::CSquareCollider(CSquareCollider&& aTemporary)
{
	self = std::move(aTemporary);
}

CSquareCollider::~CSquareCollider()
{
	SAFE_DELETE(mySquareData);
}

CSquareCollider& CSquareCollider::operator=(const CSquareCollider& aCopy)
{
	mySquareData = new Intersection::SSquare(*aCopy.mySquareData);
	return self;
}

CSquareCollider& CSquareCollider::operator=(CSquareCollider&& aTemporary)
{
	mySquareData = aTemporary.mySquareData;
	aTemporary.mySquareData = nullptr;

	return self;
}

bool CSquareCollider::TestCollision(ICollider* aCollider)
{
	return aCollider->TestCollision(this);
}

bool CSquareCollider::TestCollision(CPointCollider* aPointCollider)
{
	return Intersection::PointInsideSquare(GetData(), aPointCollider->GetData());
}

bool CSquareCollider::TestCollision(CCircleCollider* aCircleCollider)
{
	return Intersection::CircleVsSquare(aCircleCollider->GetData(), GetData());
}

bool CSquareCollider::TestCollision(CSquareCollider* aSquareCollider)
{
	return Intersection::SquareVsSquare(GetData(), aSquareCollider->GetData());
}

bool CSquareCollider::TestCollision(CTriangleCollider* aTriangleCollider)
{
	return Intersection::SquareVsTriangle(GetData(), aTriangleCollider->GetData());
}

bool CSquareCollider::TestCollision(CGroupCollider* aGroupCollider)
{
	return aGroupCollider->TestCollision(this);
}

void CSquareCollider::SetPosition(const CU::Vector3f& aPosition)
{
	SetPosition(CU::Vector2f(aPosition.x, aPosition.z));
}

void CSquareCollider::SetPosition(const CU::Vector2f aPosition)
{
	if (mySquareData != nullptr)
	{
		CU::Vector2f centerPosition = (mySquareData->myMaxPosition + mySquareData->myMinPosition) * 0.5f;

		CU::Vector2f deltaPosition = aPosition - centerPosition;

		mySquareData->myMaxPosition += deltaPosition;
		mySquareData->myMinPosition += deltaPosition;
	}
}

const Intersection::SSquare& CSquareCollider::GetData() const
{
	if (mySquareData == nullptr)
	{
		assert(!"Square data in square collider is NULL");
		return ourNullSquare;
	}

	return *mySquareData;
}
