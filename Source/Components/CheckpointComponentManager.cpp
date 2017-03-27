#include "stdafx.h"
#include "CheckpointComponentManager.h"
#include "CheckPointComponent.h"

CCheckpointComponentManager* CCheckpointComponentManager::ourInstance = nullptr;

CCheckpointComponentManager::CCheckpointComponentManager()
{
}


CCheckpointComponentManager::~CCheckpointComponentManager()
{
}

CCheckPointComponent * CCheckpointComponentManager::CreateAndRegisterComponent()
{
	static unsigned char id = 0;
	
	CCheckPointComponent* component = new CCheckPointComponent(id);

	CComponentManager::GetInstance().RegisterComponent(component);

	myCheckPointComponentList.emplace(id, component);
	id++;
	return component;
}

void CCheckpointComponentManager::CreateInstance()
{
	if(ourInstance == nullptr)
	{
		ourInstance = new CCheckpointComponentManager();
	}
}
CCheckpointComponentManager* CCheckpointComponentManager::GetInstance()
{
	return ourInstance;
}
void CCheckpointComponentManager::DestoryInstance()
{
	if(ourInstance != nullptr)
	{
		SAFE_DELETE(ourInstance);
	}
}