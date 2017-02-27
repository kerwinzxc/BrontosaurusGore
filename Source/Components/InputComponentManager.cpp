#include "stdafx.h"
#include "InputComponentManager.h"
#include "InputComponent.h"

CInputComponentManager::CInputComponentManager()
{
	myComponents.Init(2);
}


CInputComponentManager::~CInputComponentManager()
{
}

CInputComponent * CInputComponentManager::CreateAndRegisterComponent()
{
	CInputComponent* component = new CInputComponent();

	CComponentManager::GetInstance().RegisterComponent(component);

	myComponents.Add(component);

	return component;
}

void CInputComponentManager::Update()
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update();
	}
}