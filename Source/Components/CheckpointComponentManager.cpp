#include "stdafx.h"
#include "CheckpointComponentManager.h"
#include "CheckPointComponent.h"
#include "GameObject.h"

CCheckpointComponentManager* CCheckpointComponentManager::ourInstance = nullptr;

CCheckpointComponentManager::CCheckpointComponentManager()
{
}


CCheckpointComponentManager::~CCheckpointComponentManager()
{
}

CCheckPointComponent * CCheckpointComponentManager::CreateAndRegisterComponent(short aIndex)
{
	static unsigned char id = 0;
	
	CCheckPointComponent* component = new CCheckPointComponent(id, aIndex);

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

const CU::Vector3f CCheckpointComponentManager::GetNextCheckpointPosition()
{
	short lowestIndex = 1000;
	CU::Vector3f returnedVector;
	for(auto it = myCheckPointComponentList.begin(); it != myCheckPointComponentList.end(); it++)
	{
		if(it->second->GetHaveBeenActivated() == true)
		{
			continue;
		}
		if(it->second->GetIndex() < lowestIndex)
		{
			lowestIndex = it->second->GetIndex();
			returnedVector = it->second->GetParent()->GetWorldPosition();
		}
	}
	return returnedVector;
}