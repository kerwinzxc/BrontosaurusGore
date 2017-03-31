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

CSpawnerComponent::CSpawnerComponent(const CU::GrowingArray<unsigned char>& aWaves, const eEnemyTypes aEnemyType)
{
	mySpawInterval = 5.0f;
	myWaves = aWaves;

	myEnemyType = aEnemyType;

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eSpawnWave);
	//skapa wave system
	//skapa postmaster meddelanden f�r vilken wave det �r och vilka fiender som tillh�r waven
	//l�sa d�rrar till waven l�sa upp d�rr vid sista waven



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
	DL_PRINT("Spawning Enemy");
	//myEnemy->GetParent()->NotifyComponents(eComponentMessageType::eCheckPointReset, SComponentMessageData());

	myEnemy->GetParent()->NotifyComponents(eComponentMessageType::eActivate,SComponentMessageData());
	CNetworkMessage_SetIsRepesentationActive* activeMessage = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_SetIsRepesentationActive>(ID_ALL);
	activeMessage->SetActive(true);
	activeMessage->SetNetworkID(myEnemy->GetNetworkID());
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(activeMessage));
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CAddEnemyToWave(myEnemy));
}

void CSpawnerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
	{
		switch (myEnemyType)
		{
		case eEnemyTypes::eImp:
		{
			SImpBlueprint aData;
			aData.speed = 10.0f;
			aData.jumpHeight = 4.0f;
			aData.detectionRange = 30.0f;
			aData.walkToMeleeRange = 10.f;
			aData.startAttackRange = 0.0f;
			aData.stopAttackRange = 1.0f;
			myEnemy = CEnemyFactory::GetInstance()->CreateEnemy(&aData, myEnemyType, GetParent()->GetWorldPosition());
		}
		break;
		case eEnemyTypes::ePinky:
		{
			SPinkyBlueprint aData;
			aData.speed = 10.0f;
			aData.detectionRange = 30.0f;
			aData.walkToMeleeRange = 0.f;
			aData.startAttackRange = 0.0f;
			aData.stopAttackRange = 1.0f;
			myEnemy = CEnemyFactory::GetInstance()->CreateEnemy(&aData, myEnemyType, GetParent()->GetWorldPosition());
		}
		break;
		case eEnemyTypes::eRevenant:
		{
			SRevenantBlueprint aData;
			aData.speed = 10.0f;
			aData.detectionRange = 30.0f;
			aData.walkToMeleeRange = 0.f;
			aData.startAttackRange = 0.0f;
			aData.stopAttackRange = 1.0f;
			myEnemy = CEnemyFactory::GetInstance()->CreateEnemy(&aData, myEnemyType, GetParent()->GetWorldPosition());
		}
		break;
		default:
			break;
		}
		//myEnemy->GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
		//gl�m inte att l�gga in s� att charcontroller deactiverar sin collider n�r detta meddelande f�s
	}
	break;
	}
}

eMessageReturn CSpawnerComponent::DoEvent(const CStartWaveMessage & aStartWaveMessage)
{
	DL_PRINT("hej");
	for (int i = 0; i < myWaves.Size(); i++)
	{
		if (myWaves[i] == aStartWaveMessage.GetWave())
		{
			SpawnEnemy();
			break;
		}
	}

	return eMessageReturn::eContinue;
}
