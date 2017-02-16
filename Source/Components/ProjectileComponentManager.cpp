#include "stdafx.h"
#include "ProjectileComponentManager.h"
#include "ProjectileComponent.h"

ProjectileComponentManager::ProjectileComponentManager()
{
	myComponents.Init(1000);
}


ProjectileComponentManager::~ProjectileComponentManager()
{
}

ProjectileComponent * ProjectileComponentManager::CreateAndRegisterComponent()
{
	ProjectileComponent* component = new ProjectileComponent();

	CComponentManager::GetInstance().RegisterComponent(component);

	myComponents.Add(component);

	return component;
}

void ProjectileComponentManager::Update(const CU::Time& aDeltaTime)
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime.GetSeconds());
	}
}