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

CDoorComponent * CDoorManager::CreateDoorComponent(const CU::Vector2f & aOpenDirection, const bool aIsLocked,const lockID aLockId, const bool aIsClosed, const bool aShouldResetOnPlayerDeath)
{
	static int doorID = 0;
	CDoorComponent* doorComponent = new CDoorComponent();
	doorComponent->SetIsLocked(aIsLocked);
	doorComponent->SetIsClosed(aIsClosed);
	doorComponent->SetLockId(aLockId);
	doorComponent->SetNetworkID(doorID);
	doorComponent->SetShouldReset(aShouldResetOnPlayerDeath);
	COMPMGR.RegisterComponent(doorComponent);
	myDoors.emplace(doorID++, doorComponent);
	return doorComponent;
}

void CDoorManager::OpenDoor(const unsigned char aNetworkID)
{
	if (myDoors.count(aNetworkID) > 0)
	{
		myDoors.at(aNetworkID)->SetIsClosed(false);
	}
}

void CDoorManager::OpenDoor(const short aKeyId)
{
	for (auto door : myDoors)
	{
		if (door.second->GetLockId() == aKeyId)
		{
			door.second->SetIsClosed(false);
		}
	}
}

void CDoorManager::LockDoor(const unsigned char aNetworkID)
{
	if (myDoors.count(aNetworkID) > 0)
	{
		myDoors.at(aNetworkID)->SetIsLocked(true);
	}
}

void CDoorManager::LockDoor(const short aKeyId)
{
	for (auto door : myDoors)
	{
		if (door.second->GetLockId() == aKeyId)
		{
			door.second->SetIsLocked(true);
		}
	}
}

void CDoorManager::UnlockDoor(const unsigned char aNetworkID)
{
	if (myDoors.count(aNetworkID) > 0)
	{
		myDoors.at(aNetworkID)->SetIsLocked(false);
	}
}

void CDoorManager::UnlockDoor(const short aKeyId)
{
	for (auto door : myDoors)
	{
		if (door.second->GetLockId() == aKeyId)
		{
			door.second->SetIsLocked(false);
		}
	}
}

void CDoorManager::CloseDoor(const unsigned char aNetworkID)
{
	if (myDoors.count(aNetworkID) > 0)
	{
		myDoors.at(aNetworkID)->SetIsClosed(true);
	}
}

void CDoorManager::CloseDoor(const short aKeyId)
{
	for (auto door : myDoors)
	{
		if (door.second->GetLockId() == aKeyId)
		{
			door.second->SetIsClosed(true);
		}
	}
}

bool CDoorManager::DoesDoorExist(const short aKeyId, const bool aNetworked)
{
	if (aNetworked == false)
	{
		for (auto door : myDoors)
		{
			if (door.second->GetLockId() == aKeyId)
				return true;
		}
	}
	else
	{
		for (auto door : myDoors)
		{
			if (door.first == aKeyId)
				return true;
		}
	}
	return false;
}
