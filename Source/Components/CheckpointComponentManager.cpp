#include "stdafx.h"
#include "CheckpointComponentManager.h"
#include "CheckPointComponent.h"
#include "GameObject.h"

static unsigned char id = 0;
CCheckpointComponentManager* CCheckpointComponentManager::ourInstance = nullptr;

CCheckpointComponentManager::CCheckpointComponentManager()
{
	id = 0;
}


CCheckpointComponentManager::~CCheckpointComponentManager()
{
}

CCheckPointComponent * CCheckpointComponentManager::CreateAndRegisterComponent(short aIndex)
{
	
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
	else
	{
		DL_ASSERT("check point component manager already created");
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
	else
	{
		DL_ASSERT("check point component manager not created");
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