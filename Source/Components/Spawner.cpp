#include "stdafx.h"
#include "Spawner.h"
#include "EnemyFactory.h"
#include "../Components/Enemy.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TShared/NetworkMessage_SetIsRepesentationActive.h"
#include "../TServer/ServerMessageManager.h"

CSpawnerComponent::CSpawnerComponent(const CU::GrowingArray<unsigned char>& aWaves, const eEnemyTypes eEnemyType)
{
	mySpawInterval = 2.0f;
	myWaves = aWaves;
	switch (eEnemyType)
	{
	case eEnemyTypes::eImp:
	{
		SImpBlueprint aData;
		aData.speed = 10.0f;
		aData.jumpHeight = 4.0f;
		aData.detectionRange = 30.0f;
		aData.walkToMeleeRange = 0.f;
		aData.startAttackRange = 0.0f;
		aData.stopAttackRange = 1.0f;
		myEnemy = CEnemyFactory::GetInstance()->CreateEnemy(&aData, eEnemyType, GetParent()->GetWorldPosition());
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
		myEnemy = CEnemyFactory::GetInstance()->CreateEnemy(&aData, eEnemyType, GetParent()->GetWorldPosition());
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
		myEnemy = CEnemyFactory::GetInstance()->CreateEnemy(&aData, eEnemyType, GetParent()->GetWorldPosition());
	}
		break;
	default:
		break;
	}

	myEnemy->GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
}


CSpawnerComponent::~CSpawnerComponent()
{
}

void CSpawnerComponent::Update(const float aDeltaTime)
{
	static CU::TimeUnit elapsedTime = 0.0f;

	elapsedTime += aDeltaTime;
	while (elapsedTime >= mySpawInterval)
	{
		elapsedTime -= mySpawInterval;
		SpawnEnemy();
	}
}

void CSpawnerComponent::SpawnEnemy()
{
	myEnemy->GetParent()->NotifyComponents(eComponentMessageType::eActivate,SComponentMessageData());
	CNetworkMessage_SetIsRepesentationActive* activeMessage = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_SetIsRepesentationActive>(ID_ALL);
	activeMessage->SetActive(true);
	activeMessage->SetNetworkID(myEnemy->GetNetworkID());
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(activeMessage));
}
