#include "stdafx.h"
#include "ScriptComponentManager.h"
#include "ScriptComponent.h"

CScriptComponentManager* CScriptComponentManager::ourInstance = nullptr;

CScriptComponentManager::CScriptComponentManager()
	: myComponents(32u)
{
	assert(ourInstance == nullptr && "There already exists a script component manager");
	ourInstance = this;
}

CScriptComponentManager::~CScriptComponentManager()
{
	assert(ourInstance == this && "There should not exists another script component manager");
	ourInstance = nullptr;

	for (CScriptComponent* component : myComponents)
	{
		CComponentManager::GetInstance().RemoveComponent(component->GetId());
	}

	myComponents.DeleteAll();
}

CComponent* CScriptComponentManager::CreateAbstractComponent(const std::string & aScriptPath)
{
	return CreateComponent(aScriptPath);
}

CScriptComponent* CScriptComponentManager::CreateComponent(const std::string& aScriptPath)
{
	if (aScriptPath.empty()) return nullptr;

	CScriptComponent* newComponent = new CScriptComponent();
	ComponentId componentID = CComponentManager::GetInstance().RegisterComponent(newComponent);

	std::string initFunction = aScriptPath;
	initFunction -= ".lua";
	initFunction += "_init";
	
	initFunction ^= /*"Script/"*/initFunction.substr(0u, aScriptPath.find('/') + 1u);

	CScriptComponent::eInitSuccess error = newComponent->Init(aScriptPath, initFunction/*aInitFunction*/);
	if (!CScriptComponent::HandleError(error))
	{
		CComponentManager::GetInstance().DeleteComponent(componentID);
		return nullptr;
	}

	myComponents.Add(newComponent);
	return newComponent;
}

void CScriptComponentManager::DestroyComponent(CScriptComponent* aScriptComponent)
{
	myComponents.RemoveCyclic(aScriptComponent);
	CComponentManager::GetInstance().DeleteComponent(aScriptComponent->GetId());
}

CScriptComponentManager* CScriptComponentManager::GetInstance()
{
	return ourInstance;
}
