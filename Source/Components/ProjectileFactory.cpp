#include "stdafx.h"
#include "ProjectileFactory.h"
#include "ProjectileComponent.h"
#include "GameObjectManager.h"

ProjectileFactory* ProjectileFactory::ourInstance = nullptr;

ProjectileFactory::ProjectileFactory()
{
	ourInstance = this;
}


ProjectileFactory::~ProjectileFactory()
{
}

void ProjectileFactory::ShootProjectile(ProjectileData* someData, CU::Vector3f aDirection, CU::Vector3f aSpawnPosition)
{
	if(myPassiveProjectiles.Size() > 0)
	{
		myActiveProjectiles.Add(myPassiveProjectiles[0]);
		myPassiveProjectiles.RemoveCyclicAtIndex(0);
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
void ProjectileFactory::Init(CGameObjectManager* aGameObjectManager)
{
	myGameObjectManagerPointer = aGameObjectManager;
	for(unsigned short i = 0; i < 500; i++)
	{
		CreateProjectile();
	}
}

void ProjectileFactory::CreateProjectile()
{
	CGameObject* aNewProjectileObject = myGameObjectManagerPointer->CreateGameObject();
	ProjectileComponent* tempProjectileComponent = new ProjectileComponent();
	aNewProjectileObject->AddComponent(tempProjectileComponent);
	myPassiveProjectiles.Add(tempProjectileComponent);
}