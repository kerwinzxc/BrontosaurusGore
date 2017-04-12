#include "stdafx.h"
#include "ArenaTriggerComponentManager.h"
#include "TriggerArenaComponent.h"


CArenaTriggerComponentManager::CArenaTriggerComponentManager(const std::thread::id aThreadID)
{
	myComponents.Init(20);
	myThreadID = aThreadID;
}


CArenaTriggerComponentManager::~CArenaTriggerComponentManager()
{
}

void CArenaTriggerComponentManager::Update()
{
	for (int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update();
	}
}

CTriggerArenaComponent * CArenaTriggerComponentManager::CreateAndRegisterComponent(const unsigned char aNumberOfWaves, const short aKeyId, const short aArenaID)
{
	CTriggerArenaComponent* trigger = new CTriggerArenaComponent(aNumberOfWaves, aKeyId, aArenaID, myThreadID);

	myComponents.Add(trigger);

	CComponentManager::GetInstance().RegisterComponent(trigger);
	return trigger;
}
