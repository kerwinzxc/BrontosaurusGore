#include "stdafx.h"

const Intersection::SCircle CCircleCollider::ourNullCircle = {};
CU::CStaticMemoryPool<CCircleCollider, 2048> CCircleCollider::ourMemoryPool;

CCircleCollider::CCircleCollider()
	: myCircleData(nullptr)
{
	//myCircleData = new Intersection::SCircle();
}

CCircleCollider::CCircleCollider(Intersection::SCircle* someCircleData)
{
	myCircleData = someCircleData;
}

CCircleCollider::CCircleCollider(CU::Vector3f aPosition, float aRadius)
{
	myCircleData = new Intersection::SCircle();
	myCircleData->myCenterPosition = aPosition;
	myCircleData->myRadius = aRadius;
}

CCircleCollider::CCircleCollider(const CCircleCollider& aCopy)
{
	self = aCopy;
}

CCircleCollider::CCircleCollider(CCircleCollider&& aTemporary)
{
	self = std::move(aTemporary);
}

CCircleCollider::~CCircleCollider()
{
	SAFE_DELETE(myCircleData);
}

CCircleCollider& CCircleCollider::operator=(const CCircleCollider& aCopy)
{
	SAFE_DELETE(myCircleData);
	if (aCopy.myCircleData)
	{
		myCircleData = new Intersection::SCircle(*aCopy.myCircleData);
	}

	return self;
}

CCircleCollider& CCircleCollider::operator=(CCircleCollider&& aTemporary)
{
	SAFE_DELETE(myCircleData);
	myCircleData = aTemporary.myCircleData;
	aTemporary.myCircleData = nullptr;

	return self;
}

void* CCircleCollider::operator new(size_t /*aBytes*/)
{
	CCircleCollider* newCollider = ourMemoryPool.Require();
	assert(newCollider && "maybe should add dynamic memory pool too");

	return newCollider;
	//return malloc(aBytes);
}

void CCircleCollider::operator delete(void* aObject)
{
	(void)ourMemoryPool.Recycle(static_cast<CCircleCollider*>(aObject));
	//free(aObject);
}

void CCircleCollider::RenderDebugLines(CCollisionRenderer& aCollisionRenderer)
{
	Collision::CRenderCircle* renderCommand = new Collision::CRenderCircle(GetData());
	aCollisionRenderer.AddToRender(renderCommand);
}

bool CCircleCollider::TestCollision(ICollider* aCollider)
{
	return aCollider->TestCollision(this);
}

bool CCircleCollider::TestCollision(CPointCollider* aPointCollider)
{
	return Intersection::PointInsideCircle(GetData(), aPointCollider->GetData());
}

bool CCircleCollider::TestCollision(CCircleCollider* aCircleCollider)
{
	return Intersection::CircleVsCircle(GetData(), aCircleCollider->GetData());
}

bool CCircleCollider::TestCollision(CSquareCollider* aSquareCollider)
{
	assert(!"point collider not implemented");
	return false;
}

bool CCircleCollider::TestCollision(CTriangleCollider* aTriangleCollider)
{
	return Intersection::CircleVsTriangle(GetData(), aTriangleCollider->GetData());
}

bool CCircleCollider::TestCollision(CGroupCollider* aGroupCollider)
{
	return aGroupCollider->TestCollision(this);
}

void CCircleCollider::SetPosition(const CU::Vector3f& aPosition)
{
	if (myCircleData != nullptr)
	{
		myCircleData->myCenterPosition.Set(aPosition.x, aPosition.z);
	}
}

void CCircleCollider::SetPosition(const CU::Vector2f aPosition)
{
	if (myCircleData != nullptr)
	{
		myCircleData->myCenterPosition = aPosition;
	}
}

const Intersection::SCircle& CCircleCollider::GetData() const
{
	if (myCircleData == nullptr)
	{
		assert(!"Circle data in circle collider is NULL");
		return ourNullCircle;
	}

	return *myCircleData;
}
