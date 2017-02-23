#include "stdafx.h"
#include "AmmoComponentManager.h"
#include "AmmoComponent.h"


CAmmoComponentManager::CAmmoComponentManager()
{
	myComponents.Init(100);
}


CAmmoComponentManager::~CAmmoComponentManager()
{
}

CAmmoComponent * CAmmoComponentManager::CreateAndRegisterComponent()
{
	CAmmoComponent* component = new CAmmoComponent();

	CComponentManager::GetInstance().RegisterComponent(component);

	myComponents.Add(component);

	return component;
}

void CAmmoComponentManager::Update(const CU::Time& aDeltaTime)
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime.GetSeconds());
	}
}