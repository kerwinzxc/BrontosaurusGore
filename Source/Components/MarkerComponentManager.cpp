#include "stdafx.h"
#include "MarkerComponentManager.h"
#include "MarkerComponent.h"

CMarkerComponentManager* CMarkerComponentManager::ourInstance = nullptr;

CMarkerComponentManager::CMarkerComponentManager()
{
	myMarkerComponent = nullptr;
}


CMarkerComponentManager::~CMarkerComponentManager()
{
}

void CMarkerComponentManager::CreateInstance()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CMarkerComponentManager();
	}
}
CMarkerComponentManager* CMarkerComponentManager::GetInstance()
{
	return ourInstance;
}
void CMarkerComponentManager::DestroyInstance()
{
	if(ourInstance != nullptr)
	{
		SAFE_DELETE(ourInstance);
	}
}

void CMarkerComponentManager::Update(float aDeltaTime)
{
	if(myMarkerComponent != nullptr)
	{
		myMarkerComponent->Update(aDeltaTime);
	
	}
}