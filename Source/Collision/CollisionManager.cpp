#include "stdafx.h"
#include "CollisionManager.h"
#include "ICollider.h"

#ifndef _RETAIL_BUILD
#define RENDER_DEBUG_LINES
#endif // !_RETAIL_BUILD

CCollisionManager::CCollisionManager()
	: myColliders(32)
	, myCollisionRenderer(nullptr)
	, myShouldRenderDebugLines(false)
{
	myCollisionRenderer = new CCollisionRenderer();
}

CCollisionManager::~CCollisionManager()
{
	SAFE_DELETE(myCollisionRenderer);
	assert(myColliders.Size() > 0 && "Colliders still exist that are not deleted :-(");
}

void CCollisionManager::Update()
{
	for (int i = 0; i < myColliders.Size(); ++i)
	{
		for (int j = i + 1; j < myColliders.Size(); ++j)
		{
			TestCollision(myColliders[i], myColliders[j]);
		}
	}
}

void CCollisionManager::Render()
{
#ifdef RENDER_DEBUG_LINES
	if (myCollisionRenderer == nullptr) return;

	for (ICollider* collider : myColliders)
	{
		collider->RenderDebugLines(*myCollisionRenderer);
	}
#endif // RENDER_DEBUG_LINES
}

void CCollisionManager::AddCollider(ICollider* aCollider)
{
	if (aCollider == nullptr)
	{
		assert(!"You should not add nullptr to the collision manager ;-)");
		return;
	}

	assert(myColliders.Find(aCollider) == myColliders.FoundNone && "Adding collider that already exists :O");

	myColliders.Add(aCollider);
}

void CCollisionManager::RemoveCollider(ICollider* aCollider)
{
	int index = myColliders.Find(aCollider);
	if (index != myColliders.FoundNone)
	{
		auto& collidedWith = aCollider->GetCollidedWith();
		for (const ICollider* collider : collidedWith)
		{
			const_cast<ICollider*>(collider)->GetCollidedWith().Remove(aCollider);
		}

		collidedWith.RemoveAll();

		myColliders.RemoveCyclicAtIndex(index);
	}
}

const CU::GrowingArray<char, unsigned int, false>& CCollisionManager::GetLineVertexBuffer()
{
	return myCollisionRenderer->PrepareBuffer();
}

void CCollisionManager::TestCollision(ICollider* aFirst, ICollider* aSecond)
{
	if (ICollider::CanCollide(aFirst, aSecond) == false)
	{
		return;
	}

	if (aFirst->TestCollision(aSecond) == true)
	{
		if (ICollider::HasCollided(aFirst, aSecond) == false)
		{
			//DL_PRINT("on collision enter!");
			aFirst->OnCollisionEnter(aSecond);
			aSecond->OnCollisionEnter(aFirst);
		}
	}
	else
	{
		if (ICollider::HasCollided(aFirst, aSecond) == true)
		{
			//DL_PRINT("on collision exit!");
			aFirst->OnCollisionExit(aSecond);
			aSecond->OnCollisionExit(aFirst);
		}
	}
}
