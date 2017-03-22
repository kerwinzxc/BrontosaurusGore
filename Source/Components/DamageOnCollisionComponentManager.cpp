#include "stdafx.h"
#include "DamageOnCollisionComponentManager.h"
#include "DamageOnCollisionComponent.h"

CDamageOnCollisionComponentManager::CDamageOnCollisionComponentManager()
{
	myComponents.Init(100);
}


CDamageOnCollisionComponentManager::~CDamageOnCollisionComponentManager()
{
}


CDamageOnCollisionComponent * CDamageOnCollisionComponentManager::CreateAndRegisterComponent()
{
	CDamageOnCollisionComponent* component = new CDamageOnCollisionComponent();

	CComponentManager::GetInstance().RegisterComponent(component);

	myComponents.Add(component);

	return component;
}

void CDamageOnCollisionComponentManager::Update(const CU::Time& aDeltaTime)
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime.GetSeconds());
	}
}