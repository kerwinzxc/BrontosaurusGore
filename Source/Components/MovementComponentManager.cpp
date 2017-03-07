#include "stdafx.h"
#include "MovementComponentManager.h"
#include "../Components/MovementComponent.h"
#include "../Components/ComponentManager.h"


CMovementComponentManager::CMovementComponentManager()
{
	myComponents.Init(100);
}


CMovementComponentManager::~CMovementComponentManager()
{
}

CMovementComponent * CMovementComponentManager::CreateAndRegisterComponent()
{
	CMovementComponent* component = new CMovementComponent();
	CComponentManager::GetInstance().RegisterComponent(component);
	myComponents.Add(component);
	return component;
}

void CMovementComponentManager::Update(CU::Time aDeltaTime)
{
	for (int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}
