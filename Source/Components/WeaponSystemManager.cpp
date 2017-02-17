#include "stdafx.h"
#include "WeaponSystemManager.h"
#include "WeaponSystemComponent.h"


WeaponSystemManager::WeaponSystemManager(WeaponFactory* aWeaponFactoryPointer)
{
	myWeaponFactoryPointer = aWeaponFactoryPointer;
	myComponents.Init(100);
}


WeaponSystemManager::~WeaponSystemManager()
{
}

WeaponSystemComponent * WeaponSystemManager::CreateAndRegisterComponent()
{
	WeaponSystemComponent* component = new WeaponSystemComponent(*myWeaponFactoryPointer);

	CComponentManager::GetInstance().RegisterComponent(component);

	myComponents.Add(component);

	return component;
}

void WeaponSystemManager::Update(const CU::Time& aDeltaTime)
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime.GetSeconds());
	}
}