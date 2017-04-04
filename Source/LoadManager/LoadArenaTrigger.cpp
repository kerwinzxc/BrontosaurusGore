#include "stdafx.h"
#include "LoadArenaTrigger.h"
#include "../Components/TriggerArenaComponent.h"
#include "../Components/ComponentManager.h"

int LoadArenaTrigger(KLoader::SLoadedComponentData someData)
{
	CTriggerArenaComponent* trigger = new CTriggerArenaComponent(someData.myData.at("NumberOfWaves").GetUchar(),someData.myData.at("LockID").GetShort());

	CComponentManager::GetInstance().RegisterComponent(trigger);

	return trigger->GetId();
}
