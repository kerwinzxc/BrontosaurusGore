#include "stdafx.h"
#include "Spawner.h"
#include "EnemyFactory.h"
#include "../Components/Enemy.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../ThreadedPostmaster/StartWaveMessage.h"
#include "../ThreadedPostmaster/AddEnemyToWave.h"
#include "../TShared/NetworkMessage_SetIsRepesentationActive.h"
#include "../TServer/ServerMessageManager.h"
#include "EnemyTypes.h"

CSpawnerComponent::CSpawnerComponent(const CU::GrowingArray<unsigned short>& aWaves, const eEnemyTypes aEnemyType, std::thread::id aID)
{
	mySpawInterval = 5.0f;
	myWaves = aWaves;

	myEnemyType = aEnemyType;

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eSpawnWave, aID);
	//skapa wave system
	//skapa postmaster meddelanden för vilken wave det är och vilka fiender som tillhör waven
	//låsa dörrar till waven låsa upp dörr vid sista waven



}


CSpawnerComponent::~CSpawnerComponent()
{
}

void CSpawnerComponent::Update(const float aDeltaTime)
{
	static CU::TimeUnit elapsedTime = mySpawInterval;

	//elapsedTime += aDeltaTime;
	while(elapsedTime >= mySpawInterval)
	{
		elapsedTime -= mySpawInterval;
		SpawnEnemy();
	}
}

void CSpawnerComponent::SpawnEnemy()
{
	//DL_PRINT("Spawning Enemy");
	/*myEnemy->GetParent()->NotifyComponents(eComponentMessageType::eCheckPointReset, SComponentMessageData());

	myEnemy->GetParent()->NotifyComponents(eComponentMessageType::eActivate,SComponentMessageData());*/

	//skicka ett meddelande som revivar repensentation
	//CNetworkMessage_SetIsRepesentationActive* activeMessage = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_SetIsRepesentationActive>(ID_ALL);
	//activeMessage->SetActive(true);
	//activeMessage->SetNetworkID(myEnemy->GetNetworkID());
	//Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(activeMessage));

	myEnemy = CEnemyFactory::GetInstance()->CreateEnemy(myEnemyType, GetParent()->GetWorldPosition());

	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAddEnemyToWave(myEnemy));
}

void CSpawnerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
	{
	
		//myEnemy->GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
		//glöm inte att lägga in så att charcontroller deactiverar sin collider när detta meddelande fås
	}
	break;
	}
}

eMessageReturn CSpawnerComponent::DoEvent(const CStartWaveMessage & aStartWaveMessage)
{
	for (int i = 0; i < myWaves.Size(); i++)
	{
		//unsigned short it = myWaves[i];
		if (myWaves[i] == aStartWaveMessage.GetWave())
		{
			SpawnEnemy();
			break;
		}
	}

	return eMessageReturn::eContinue;
}
