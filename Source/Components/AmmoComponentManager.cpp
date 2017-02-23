#include "stdafx.h"
#include "AmmoComponentManager.h"
#include "AmmoComponent.h"


AmmoComponentManager::AmmoComponentManager()
{
	myComponents.Init(100);
}


AmmoComponentManager::~AmmoComponentManager()
{
}

AmmoComponent * AmmoComponentManager::CreateAndRegisterComponent()
{
	AmmoComponent* component = new AmmoComponent();

	CComponentManager::GetInstance().RegisterComponent(component);

	myComponents.Add(component);

	return component;
}

void AmmoComponentManager::Update(const CU::Time& aDeltaTime)
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime.GetSeconds());
	}
}