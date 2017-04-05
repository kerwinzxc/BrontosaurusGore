#include "stdafx.h"
#include "ProjectileFactory.h"
#include "ProjectileComponent.h"
#include "GameObjectManager.h"
#include "ProjectileComponentManager.h"
#include "ModelComponentManager.h"
#include "ModelComponent.h"
#include "ProjectileData.h"
#include "ProjectileBufferData.h"
#include "ColliderComponentManager.h"
#include "../Physics/PhysicsCharacterController.h"
#include "CharacterControllerComponent.h"
#include "../Physics/CollisionLayers.h"
#include "../Physics/Shape.h"

CProjectileFactory* CProjectileFactory::ourInstance = nullptr;

CProjectileFactory::CProjectileFactory(CProjectileComponentManager* aProjectileComponentManager)
{
	ourInstance = this;
	myProjectileComponentManager = aProjectileComponentManager;
}


CProjectileFactory::~CProjectileFactory()
{
	ourInstance = nullptr;
}

void CProjectileFactory::ShootProjectile(SProjectileData* someData,const CU::Vector3f& aDirection,const CU::Vector3f& aSpawnPosition)
{
	for(unsigned int i = 0; i < myPassiveProjectiles.Size(); i++)
	{
		if(myPassiveProjectiles[i]->projectileName == someData->projectileModelFilePath) // stuff like this really needs to get hashed
		{
			ShootProjectile(someData, aDirection, aSpawnPosition, i);
			return;
		}
	}
	DL_PRINT("Refactoring gone wrong in ProjectileFactory yell at Marcus");
}
void CProjectileFactory::ShootProjectile(SProjectileData* someData,const CU::Vector3f& aDirection,const CU::Vector3f& aSpawnPosition, unsigned int aIndex)
{
	if (myPassiveProjectiles[aIndex]->projectileBuffer.Size() > 0)
	{
		myActiveProjectiles[aIndex]->projectileBuffer.Add(myPassiveProjectiles[aIndex]->projectileBuffer[0]);
		myPassiveProjectiles[aIndex]->projectileBuffer.RemoveCyclicAtIndex(0);
		myActiveProjectiles[aIndex]->projectileBuffer.GetLast()->Activate(someData, aDirection, aSpawnPosition);
	}
	else
	{
		DL_PRINT("Projectile buffer to small please increase it! Yell at Marcus if this causes you major frame drops");
		for (unsigned short i = 0; i < 20; i++)
		{
			CreateProjectile(aIndex);
		}
		ShootProjectile(someData, aDirection, aSpawnPosition);
	}
}
void CProjectileFactory::Update(float aDeltaTime)
{
	for(unsigned short i = 0; i < myActiveProjectiles.Size(); i++)
	{
		for(unsigned short j = 0; j < myActiveProjectiles[i]->projectileBuffer.Size(); j++)
		{
			if(myActiveProjectiles[i]->projectileBuffer[j]->GetIsActive() == false)
			{
				myPassiveProjectiles[i]->projectileBuffer.Add(myActiveProjectiles[i]->projectileBuffer[j]);
				myActiveProjectiles[i]->projectileBuffer.RemoveCyclicAtIndex(j);
				j--;
				continue;
			}
		}
	}
}
void CProjectileFactory::Init(CGameObjectManager* aGameObjectManager, CModelComponentManager* aModelComponentManagerPointer, CColliderComponentManager* aColliderComponentManager)
{
	myGameObjectManagerPointer = aGameObjectManager;
	myActiveProjectiles.Init(5);
	myPassiveProjectiles.Init(5);
	myModelComponentManagerPointer = aModelComponentManagerPointer;
	myColliderComponentManagerPointer = aColliderComponentManager;
	//for(unsigned short i = 0; i < 500; i++)
	//{
	//	CreateProjectile();
	//}
}

void CProjectileFactory::CreateProjectile(unsigned int aIndex)
{
	CGameObject* newProjectileObject = myGameObjectManagerPointer->CreateGameObject();
	newProjectileObject->GetLocalTransform().Scale(CU::Vector3f(0.1, 0.1, 0.1));
	CProjectileComponent* tempProjectileComponent = myProjectileComponentManager->CreateAndRegisterComponent();
	newProjectileObject->AddComponent(tempProjectileComponent);
	if (!myPassiveProjectiles[aIndex]->projectileName.empty())
	{
		//BUGG: carl did this, don't know if it causes problems
		CModelComponent* modelComponent = myModelComponentManagerPointer->CreateComponent(myPassiveProjectiles[aIndex]->projectileName.c_str());
		newProjectileObject->AddComponent(modelComponent);
	}

	newProjectileObject->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	SComponentMessageData visibilityData;
	visibilityData.myBool = false;
	newProjectileObject->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, visibilityData);

	SSphereColliderData sphereColliderDesc;
	sphereColliderDesc.myRadius = 1.0f;
	sphereColliderDesc.IsTrigger = false;

	unsigned int collideWith = Physics::CollideEverything;
	collideWith &= ~Physics::ECollisionLayer::eProjectile;
	sphereColliderDesc.myLayer = Physics::eProjectile;
	sphereColliderDesc.myCollideAgainst = static_cast<Physics::ECollisionLayer>(collideWith);

	
	CColliderComponent* projectileSphereCollider = myColliderComponentManagerPointer->CreateComponent(&sphereColliderDesc);
	newProjectileObject->AddComponent(projectileSphereCollider);
	
	SRigidBodyData rigidbodyData;
	rigidbodyData.isKinematic = false;
	rigidbodyData.useGravity = false;
	CColliderComponent* projectileRigidBodyCollider = myColliderComponentManagerPointer->CreateComponent(&rigidbodyData);
	
	newProjectileObject->AddComponent(projectileRigidBodyCollider);

	newProjectileObject->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
	myPassiveProjectiles[aIndex]->projectileBuffer.Add(tempProjectileComponent);
}

void CProjectileFactory::CreateNewProjectileBuffer(std::string aProjctileName, unsigned int aSuggestedProjectileBufferSize)
{
	for(unsigned int i = 0; i < myPassiveProjectiles.Size(); i++)
	{
		if(myPassiveProjectiles[i]->projectileName == aProjctileName) // stuff like this really needs to get hashed
		{
			return;
		}
	}

	SProjectileBufferData* newBufferDataPassive = new SProjectileBufferData;
	newBufferDataPassive->projectileName = aProjctileName;
	newBufferDataPassive->projectileBuffer.Init(aSuggestedProjectileBufferSize);
	SProjectileBufferData* newBufferDataActive = new SProjectileBufferData;
	newBufferDataActive->projectileName = aProjctileName;
	newBufferDataActive->projectileBuffer.Init(aSuggestedProjectileBufferSize);
	myPassiveProjectiles.Add(newBufferDataPassive);
	myActiveProjectiles.Add(newBufferDataActive);
	for(unsigned int i = 0; i < aSuggestedProjectileBufferSize; i++)
	{
		CreateProjectile(myActiveProjectiles.Size() - 1);
	}
}