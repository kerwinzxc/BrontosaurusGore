#include "stdafx.h"
#include "WeaponSystemManager.h"
#include "WeaponSystemComponent.h"


CWeaponSystemManager::CWeaponSystemManager(CWeaponFactory* aWeaponFactoryPointer)
{
	myWeaponFactoryPointer = aWeaponFactoryPointer;
	myComponents.Init(100);
}


CWeaponSystemManager::~CWeaponSystemManager()
{
}

CWeaponSystemComponent * CWeaponSystemManager::CreateAndRegisterComponent()
{
	CWeaponSystemComponent* component = new CWeaponSystemComponent(*myWeaponFactoryPointer);

	CComponentManager::GetInstance().RegisterComponent(component);

	myComponents.Add(component);

	return component;
}

void CWeaponSystemManager::Update(const CU::Time& aDeltaTime)
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime.GetSeconds());
	}
}