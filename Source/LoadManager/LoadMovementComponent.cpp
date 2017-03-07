#include "stdafx.h"
#include "LoadMovementComponent.h"
#include "../Components/MovementComponent.h"
#include "../Components/MovementComponentManager.h"


int LoadMovementComponent(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	CMovementComponent* movement = loadManager.GetCurrentPLaystate().GetMovementComponentManager()->CreateAndRegisterComponent();
	return movement->GetId();
}

int ServerLoadMovementComponent(KLoader::SLoadedComponentData someData)
{
	GET_SERVERLOADMANAGER(loadManager);
	CMovementComponent* movement = loadManager.GetCurrentGameServer().GetMovementComponentManager()->CreateAndRegisterComponent();
	return movement->GetId();
}
