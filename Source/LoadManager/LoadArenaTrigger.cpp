#include "stdafx.h"
#include "LoadArenaTrigger.h"
#include "../Components/TriggerArenaComponent.h"
#include "../Components/ComponentManager.h"
#include "../Components/ArenaTriggerComponentManager.h"

int LoadArenaTrigger(KLoader::SLoadedComponentData someData)
{

	GET_SERVERLOADMANAGER(loadManager);

	if (someData.myData.HasKey("LockID") == false)
	{
		DL_MESSAGE_BOX("Missing LockID for a ArenaTrigger, find and repair it and I will give you 5 gold coins");
	}

	if (someData.myData.HasKey("NumberOfWaves") == false)
	{
		DL_MESSAGE_BOX("Missing NumberOfWaves for a ArenaTrigger, find and repair it and I will give you a hen, free of charge");
	}

	if (someData.myData.HasKey("ArenaID") == false)
	{
		DL_MESSAGE_BOX("Missing ArenaID for a ArenaTrigger, find and repair it and I will give you a unlikey western movie, for a small fee");
	}

	CTriggerArenaComponent* trigger = loadManager.GetCurrentGameServer().GetArenaTriggerManager()->CreateAndRegisterComponent(someData.myData.at("NumberOfWaves").GetUchar(),someData.myData.at("LockID").GetShort(),someData.myData.at("ArenaID").GetShort());

	return trigger->GetId();
}
