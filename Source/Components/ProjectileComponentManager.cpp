#include "stdafx.h"
#include "ProjectileComponentManager.h"
#include "ProjectileComponent.h"

CProjectileComponentManager::CProjectileComponentManager()
{
	myComponents.Init(1000);
}


CProjectileComponentManager::~CProjectileComponentManager()
{
}

CProjectileComponent * CProjectileComponentManager::CreateAndRegisterComponent()
{
	CProjectileComponent* component = new CProjectileComponent();

	CComponentManager::GetInstance().RegisterComponent(component);

	myComponents.Add(component);

	return component;
}

void CProjectileComponentManager::Update(const CU::Time& aDeltaTime)
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime.GetSeconds());
	}
}