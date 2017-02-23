#include "stdafx.h"
#include "ProjectileFactory.h"
#include "ProjectileComponent.h"
#include "GameObjectManager.h"
#include "ProjectileComponentManager.h"
#include "ModelComponentManager.h"
#include "ModelComponent.h"
#include "ProjectileData.h"

ProjectileFactory* ProjectileFactory::ourInstance = nullptr;

ProjectileFactory::ProjectileFactory(ProjectileComponentManager* aProjectileComponentManager)
{
	ourInstance = this;
	myProjectileComponentManager = aProjectileComponentManager;
}


ProjectileFactory::~ProjectileFactory()
{
	ourInstance = nullptr;
}

void ProjectileFactory::ShootProjectile(ProjectileData* someData, CU::Vector3f aDirection, CU::Vector3f aSpawnPosition)
{
	if(myPassiveProjectiles.Size() > 0)
	{
		myActiveProjectiles.Add(myPassiveProjectiles[0]);
		myPassiveProjectiles.RemoveCyclicAtIndex(0);
		myActiveProjectiles.GetLast()->Activate(someData, aDirection, aSpawnPosition);
		CModelComponent* modelComponent = myModelComponentManagerPointer->CreateComponent(someData->projectileModelFilePath.c_str());
		myActiveProjectiles.GetLast()->GetParent()->AddComponent(modelComponent);
		myActiveProjectiles.GetLast()->GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	}
	else
	{
		DL_PRINT("Projectile buffer to small please increase it! Yell at Marcus if this causes you major frame drops");
		for (unsigned short i = 0; i < 20; i++)
		{
			CreateProjectile();
		}
		ShootProjectile(someData, aDirection, aSpawnPosition);
	}
}
void ProjectileFactory::Update(float aDeltaTime)
{
	for(unsigned short i = 0; i < myActiveProjectiles.Size(); i++)
	{
		myActiveProjectiles[i]->Update(aDeltaTime);
	}
}
void ProjectileFactory::Init(CGameObjectManager* aGameObjectManager, CModelComponentManager* aModelComponentManagerPointer)
{
	myGameObjectManagerPointer = aGameObjectManager;
	myActiveProjectiles.Init(500);
	myPassiveProjectiles.Init(500);
	myModelComponentManagerPointer = aModelComponentManagerPointer;
	for(unsigned short i = 0; i < 500; i++)
	{
		CreateProjectile();
	}
}

void ProjectileFactory::CreateProjectile()
{
	CGameObject* aNewProjectileObject = myGameObjectManagerPointer->CreateGameObject();
	ProjectileComponent* tempProjectileComponent = myProjectileComponentManager->CreateAndRegisterComponent();
	aNewProjectileObject->AddComponent(tempProjectileComponent);
	myPassiveProjectiles.Add(tempProjectileComponent);
}