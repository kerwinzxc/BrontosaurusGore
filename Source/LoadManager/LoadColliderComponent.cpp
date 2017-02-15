#include "stdafx.h"
#include "LoadColliderComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"

#include <Collision/Intersection.h>
#include "CollisionComponent.h"
#include "CollisionComponentManager.h"
#include "Collision/ICollider.h"

int LoadCircleCollider(KLoader::SLoadedComponentData someData)
{
	CPlayState* playState = LoadManager::GetInstance().GetCurrentPLaystate();
	if (playState == nullptr) return 0;

	CCollisionComponentManager* collisionManager = playState->GetCollisionManager();
	if (collisionManager == nullptr) return 0;

	//collect json-data
	float circleRadius = someData.myData.at("Radius").GetFloat();
	unsigned int colliderType = someData.myData.at("ColliderType").GetUInt();
	const CJsonValue& collidesWithArray = someData.myData.at("CollidesWith");
	unsigned int collidesWith = 0;
	for (int i = 0; i < collidesWithArray.Size(); ++i)
	{
		collidesWith |= collidesWithArray[i].GetUInt();
	}


	Intersection::CollisionData collisionData;
	collisionData.myCircleData = new Intersection::SCircle();

	collisionData.myCircleData->myRadius = circleRadius;

	CCollisionComponent* collisionComponent = collisionManager->CreateCollisionComponent(CCollisionComponentManager::eColliderType::eCircle, collisionData);
	collisionComponent->SetColliderType(static_cast<eColliderType>(colliderType));
	collisionComponent->AddCollidsWith(collidesWith);

	return collisionComponent->GetId();
}

int LoadSquareCollider(KLoader::SLoadedComponentData someData)
{
	return 0;
}

int LoadPointCollider(KLoader::SLoadedComponentData someData)
{
	return 0;
}
