#include "stdafx.h"
#include "CheckpoinComponenttManager.h"
#include "CheckPointComponent.h"

CCheckpoinComponentManager* CCheckpoinComponentManager::ourInstance = nullptr;

CCheckpoinComponentManager::CCheckpoinComponentManager()
{
}


CCheckpoinComponentManager::~CCheckpoinComponentManager()
{
}

CCheckPointComponent * CCheckpoinComponentManager::CreateAndRegisterComponent()
{
	static unsigned char id = 0;
	
	CCheckPointComponent* component = new CCheckPointComponent(id);

	CComponentManager::GetInstance().RegisterComponent(component);

	myCheckPointComponentList.emplace(id, component);
	id++;
	return component;
}

void CCheckpoinComponentManager::CreateInstance()
{
	if(ourInstance == nullptr)
	{
		ourInstance = new CCheckpoinComponentManager();
	}
}
CCheckpoinComponentManager* CCheckpoinComponentManager::GetInstance()
{
	return ourInstance;
}
void CCheckpoinComponentManager::DestoryInstance()
{
	if(ourInstance != nullptr)
	{
		SAFE_DELETE(ourInstance);
	}
}