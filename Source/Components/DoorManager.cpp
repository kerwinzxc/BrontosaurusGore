#include "stdafx.h"
#include "DoorManager.h"
#include "DoorComponent.h"
#include "ComponentManager.h"

CDoorManager* CDoorManager::ourInstance = nullptr;

CDoorManager::CDoorManager()
{
}


CDoorManager::~CDoorManager()
{
}

void CDoorManager::Create()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CDoorManager();
	}
}

void CDoorManager::Destroy()
{
	if (ourInstance != nullptr)
	{
		SAFE_DELETE(ourInstance);
	}
}

CDoorManager* CDoorManager::GetInstance()
{
	return ourInstance;
}

void CDoorManager::Update(const CU::Time & aDeltaTime)
{
	for (auto door : myDoors)
	{
		door.second->Update(aDeltaTime);
	}
}

CDoorComponent * CDoorManager::CreateDoorComponent(const CU::Vector2f & aOpenDirection, const bool aIsLocked,const lockID aLockId)
{
	static int doorID = 0;
	CDoorComponent* doorComponent = new CDoorComponent();
	doorComponent->SetIsLocked(aIsLocked);
	doorComponent->SetIsClosed(true);
	doorComponent->SetLockId(aLockId);
	doorComponent->SetNetworkID(doorID);
	COMPMGR.RegisterComponent(doorComponent);
	myDoors.emplace(doorID++, doorComponent);
	return doorComponent;
}

void CDoorManager::OpenDoor(const unsigned char aNetworkID)
{
	myDoors.at(aNetworkID)->SetIsClosed(false);
}

void CDoorManager::UnlockDoor(const unsigned char aNetworkID)
{
	myDoors.at(aNetworkID)->SetIsLocked(false);
}

void CDoorManager::CloseDoor(const unsigned char aNetworkID)
{
	myDoors.at(aNetworkID)->SetIsClosed(true);
}
