#include "stdafx.h"
#include "PhysicsManager.h"
#include "../Components/ComponentManager.h"
#include "RigidBodyComponent.h"

CPhysicsManager* CPhysicsManager::ourInstance = nullptr;
int CPhysicsManager::ourRefCount = 0;

CPhysicsManager::CPhysicsManager()
{

}


CPhysicsManager::~CPhysicsManager()
{

}

bool CPhysicsManager::CreateInstance()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CPhysicsManager();
	}
	ourRefCount++;
	return true;
}

CPhysicsManager& CPhysicsManager::GetInstance()
{
	if (ourInstance == nullptr)
	{
		DL_ASSERT("PhysicsManager not created");
	}

	return *ourInstance;
}

void CPhysicsManager::DestroyInstance()
{
	if (ourInstance != nullptr)
	{
		ourRefCount--;

		if (ourRefCount < 1)
		{
			delete ourInstance;
			ourInstance = nullptr;
		}
	}
}

void CPhysicsManager::Update()
{

}
