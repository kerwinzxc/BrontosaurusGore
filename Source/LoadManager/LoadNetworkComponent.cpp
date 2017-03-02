#include "stdafx.h"
#include "LoadNetworkComponent.h"
#include "../Components/ComponentManager.h"
#include "../Components/NetworkComponent.h"
#include "../Components/NetworkComponentManager.h"
#include "ServerLoadManager.h"
#include "../TServer/GameServer.h"

int LoadClientNetworkComponent(KLoader::SLoadedComponentData someData)
{
	static NetworkId id = 0;
	CNetworkComponent* networkComponent = LoadManager::GetInstance()->GetCurrentPLaystate().GetNetworkComponentManager()->CreateAndRegisterComponent(id);

	return networkComponent->GetId();
}

int LoadServerNetworkComponent(KLoader::SLoadedComponentData someData)
{
	static NetworkId id = 0;
	CNetworkComponent* networkComponent = CServerLoadManager::GetInstance()->GetCurrentGameServer().GetNetworkComponentManager().CreateAndRegisterComponent(id);

	return networkComponent->GetId();
}
