#include "stdafx.h"
#include "GroupCollider.h"

CGroupCollider::CGroupCollider()
	: myColliders(4ui16/*having fun mvh carl*/)
{
}

CGroupCollider::CGroupCollider(const CGroupCollider& aCopy)
{
	self = aCopy;
}

CGroupCollider::CGroupCollider(CGroupCollider&& aTemporary)
{
	self = std::move(aTemporary);
}

CGroupCollider::~CGroupCollider()
{
	myColliders.DeleteAll();
}

CGroupCollider& CGroupCollider::operator=(const CGroupCollider& aCopy)
{
	myColliders = aCopy.myColliders;
	return self;
}

CGroupCollider& CGroupCollider::operator=(CGroupCollider&& aTemporary)
{
	myColliders = std::move(aTemporary.myColliders);
	return self;
}

void CGroupCollider::RenderDebugLines(CCollisionRenderer& aCollisionRenderer)
{
	for (ICollider* collider : myColliders)
	{
		collider->RenderDebugLines(aCollisionRenderer);
	}
}

bool CGroupCollider::TestCollision(ICollider* aCollider)
{
	return aCollider->TestCollision(this);
}

bool CGroupCollider::TestCollision(CPointCollider* aPointCollider)
{
	for (ICollider* collider : myColliders)
	{
		if (collider->TestCollision(aPointCollider) == true)
		{
			return true;
		}
	}

	return false;
}

bool CGroupCollider::TestCollision(CCircleCollider* aCircleCollider)
{
	for (ICollider* collider : myColliders)
	{
		if (collider->TestCollision(aCircleCollider) == true)
		{
			return true;
		}
	}

	return false;
}

bool CGroupCollider::TestCollision(CSquareCollider* aSquareCollider)
{
	for (ICollider* collider : myColliders)
	{
		if (collider->TestCollision(aSquareCollider) == true)
		{
			return true;
		}
	}

	return false;
}

bool CGroupCollider::TestCollision(CGroupCollider* aGroupCollider)
{
	for (ICollider* collider : myColliders)
	{
		if (collider->TestCollision(aGroupCollider) == true)
		{
			return true;
		}
	}

	return false;
}

void CGroupCollider::SetPosition(const CU::Vector3f& aPosition)
{
	aPosition;
	assert(!"Must implement a parent position system for this, otherwise group collider is worthless");
}

void CGroupCollider::SetPosition(const CU::Vector2f aPosition)
{
	aPosition;
	assert(!"Must implement a parent position system for this, otherwise group collider is worthless");
}
