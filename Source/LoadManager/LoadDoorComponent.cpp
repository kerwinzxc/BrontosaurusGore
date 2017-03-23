#include "stdafx.h"
#include "LoadDoorComponent.h"
#include "DoorManager.h"
#include "DoorComponent.h"

int LoadDoorComponent(KLoader::SLoadedComponentData someData)
{
	const bool isLocked = someData.myData.at("IsLocked").GetBool();
	const bool isClosed = someData.myData.at("IsClosed").GetBool();
	//TODO: Return this
	//const bool shoudlReset = someData.myData.at("ResetOnPlayerDeath").GetBool();
	const lockID lockId = someData.myData.at("LockId").GetInt();

	CDoorComponent* doorComponent = CDoorManager::GetInstance()->CreateDoorComponent(CU::Vector2f::Zero, isLocked, lockId, isClosed,false);
	return doorComponent->GetId();
}
