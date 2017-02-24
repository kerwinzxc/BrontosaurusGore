#include "stdafx.h"
#include "CollisionComponentManager.h"
#include "CollisionComponent.h"

#include "../Collision/CollisionManager.h"

#include "../Collision/PointCollider.h"
#include "../Collision/SquareCollider.h"
#include "../Collision/CircleCollider.h"
#include "../Collision/GroupCollider.h"

#include "../Collision/Intersection.h"
#include "ComponentManager.h"

#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/Renderer.h"

CCollisionComponentManager::CCollisionComponentManager()
	: myCollisionComponents(32)
	, myCollisionManager(nullptr)
	, myShouldRender(false)
{
	myCollisionManager = new CCollisionManager();

#ifdef _DEBUG
	myShouldRender = true;
#endif // _DEBUG
}


CCollisionComponentManager::~CCollisionComponentManager()
{
	//myCollisionComponents.DeleteAll();
	SAFE_DELETE(myCollisionManager);
}

void CCollisionComponentManager::Update()
{
	myCollisionManager->Update();

	if (myShouldRender == true)
	{
		myCollisionManager->Render();
	}
}

void CCollisionComponentManager::Render()
{
	if (myShouldRender == true)
	{
		const CU::GrowingArray<char, unsigned int, false>& lineVertexBuffer = myCollisionManager->GetLineVertexBuffer();
		CRenderer& renderer = RENDERER;

		SRenderLineBuffer* renderCollisionMessage = new SRenderLineBuffer(lineVertexBuffer);
		renderer.AddRenderMessage(renderCollisionMessage);
	}
}

CCollisionComponent* CCollisionComponentManager::CreateCollisionComponent(const eColliderType aColliderType, Intersection::CollisionData& aCollisionData)
{
	ICollider* newCollider = CreateCollider(aColliderType, aCollisionData);
	myCollisionManager->AddCollider(newCollider);

	auto activateCallback = [this, newCollider]()
	{
		myCollisionManager->AddCollider(newCollider);
	};
	auto deactivateCallback = [this, newCollider]()
	{
		myCollisionManager->RemoveCollider(newCollider);
	};

	newCollider->AddActivationCallbacks(activateCallback, deactivateCallback);

	CCollisionComponent* newCollisionComponent = new CCollisionComponent(newCollider);

	myCollisionComponents.Add(newCollisionComponent);

	CComponentManager::GetInstance().RegisterComponent(newCollisionComponent);

	return newCollisionComponent;
}

void CCollisionComponentManager::DestroyCollisionComponent(CCollisionComponent* aCollisionComponent)
{
	unsigned int index = myCollisionComponents.Find(aCollisionComponent);
	if (index != myCollisionComponents.FoundNone)
	{
		myCollisionManager->RemoveCollider(aCollisionComponent->GetCollider());
		//CComponentManager::GetInstance().DeleteComponent(aCollisionComponent->GetId());
		//myCollisionComponents.DeleteCyclicAtIndex(index);
	}
}

ICollider* CCollisionComponentManager::CreateCollider(const eColliderType aColliderType, Intersection::CollisionData& aCollisionData)
{
	ICollider* newCollider = nullptr;

	switch (aColliderType)
	{
	case CCollisionComponentManager::eColliderType::eCircle:
		newCollider = new CCircleCollider(aCollisionData.myCircleData);
		break;
	case CCollisionComponentManager::eColliderType::ePoint:
		newCollider = new CPointCollider();
		break;
	case CCollisionComponentManager::eColliderType::eSquare:
		newCollider = new CSquareCollider();
		break;
	}

	return newCollider;
}

void CCollisionComponentManager::FlipShouldRender()
{
	myShouldRender = !myShouldRender;
}
