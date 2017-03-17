#include "stdafx.h"
#include "ExplosionComponentManager.h"
#include "ExplosionComponent.h"

CExplosionComponentManager::CExplosionComponentManager()
{
	myComponents.Init(10);
}


CExplosionComponentManager::~CExplosionComponentManager()
{
}

CExplosionComponent * CExplosionComponentManager::CreateAndRegisterComponent()
{
	CExplosionComponent* component = new CExplosionComponent();

	CComponentManager::GetInstance().RegisterComponent(component);

	myComponents.Add(component);

	return component;
}

void CExplosionComponentManager::Update(const CU::Time& aDeltaTime)
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime.GetSeconds());
	}
}