#include "stdafx.h"
#include "WaveManager.h"
#include "../Components/Enemy.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/AddEnemyToWave.h"
#include "../ThreadedPostmaster/StartWaveMessage.h"
#include "../ThreadedPostmaster/PlayerEnteredArena.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TShared/NetworkMessage_DoorMessage.h"
#include "../TServer/ServerMessageManager.h"
#include "../Game/PollingStation.h"
#include "../Components/Enemy.h"
#include "../Components/GameObject.h"
#include "../Components/ComponentMessage.h"
#include "../TShared/NetworkMessage_SetIsRepesentationActive.h"

CWaveManager::CWaveManager()
{
	myEnemiesInWave.Init(30);

	 myKeyIDToUnlock = 0;

	myPlayersInsideArena = 0;
	myWaveCount = 0;
	myNumberOfPlayers = 0;
	myNumberOfWavesToSpawn = 0;
	myResetToWaveCount = 0;
	myResetToNumberOfWaves = 0;

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::ePlayerEnterArena);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eAddEnemyToWave);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eResetToCheckPointMessage);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eSetNewCheckPoint);

}


CWaveManager::~CWaveManager()
{
}

void CWaveManager::StartWave()
{
	DL_PRINT("TotalWaves:");
	DL_PRINT(std::to_string(myNumberOfWavesToSpawn).c_str());
	if (myWaveCount < myNumberOfWavesToSpawn)
	{
		myWaveCount++;
		DL_PRINT("Wave:");
		DL_PRINT(std::to_string(myWaveCount).c_str());
		//DL_PRINT("BroadCast StartWave");
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CStartWaveMessage(myWaveCount));
	}
	else
	{
		DL_PRINT("Enemies defeated Unlocks door");
		//myResetToWaveCount = myWaveCount;
		//myResetToNumberOfWaves = myNumberOfWavesToSpawn;
 		CNetworkMessage_DoorMessage* door = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_DoorMessage>(ID_ALL);
		door->SetDoorAction(eDoorAction::eUnlock);
		door->SetKeyID(myKeyIDToUnlock);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(door));
		CNetworkMessage_DoorMessage* door2 = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_DoorMessage>(ID_ALL);
		door2->SetDoorAction(eDoorAction::eOpen);
		door2->SetKeyID(myKeyIDToUnlock);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(door2));

	}
}

void CWaveManager::Update()
{
	SComponentMessageData data; data.myInt = 10000;
	for (unsigned int i = 0; i < myEnemiesInWave.Size(); i++)
	{
		if (myEnemiesInWave[i]->GetParent()->GetLocalTransform().GetPosition().y < -150)
		{
			myEnemiesInWave[i]->GetParent()->NotifyComponents(eComponentMessageType::eDied, data);
		}
		if (myEnemiesInWave[i]->GetIsDead() == true)
		{
			myEnemiesInWave.Remove(myEnemiesInWave[i]);
			if (myEnemiesInWave.Empty() == true)
			{
				StartWave();
			}
		}
	}
}

eMessageReturn CWaveManager::DoEvent(const CAddEnemyToWave & aAddEnemyToWave)
{
	DL_PRINT("EnemyAdded");
	myEnemiesInWave.Add(aAddEnemyToWave.GetEnemy());

	return eMessageReturn::eContinue;
}

eMessageReturn CWaveManager::DoEvent(const CPlayerEnteredArena & aPlayerEnteredArena)
{

	DL_PRINT("WaveManager: PlayerEntered");
    myPlayersInsideArena += aPlayerEnteredArena.GetPlayerChange();
	myKeyIDToUnlock = aPlayerEnteredArena.GetKeyId();

	//myResetToNumberOfWaves = myNumberOfWavesToSpawn;
	//myResetToWaveCount = myWaveCount;

	myNumberOfWavesToSpawn += aPlayerEnteredArena.GetWaveAmount();

	if (myPlayersInsideArena >= CPollingStation::GetInstance()->GetNumberOfPlayers())
	{
		CNetworkMessage_DoorMessage* door = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_DoorMessage>(ID_ALL);
		door->SetDoorAction(eDoorAction::eClose);
		door->SetKeyID(myKeyIDToUnlock);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(door));
		CNetworkMessage_DoorMessage* door2 = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_DoorMessage>(ID_ALL);
		door2->SetDoorAction(eDoorAction::eLock);
		door2->SetKeyID(myKeyIDToUnlock);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(door2));
		myEnemiesInWave.RemoveAll();
		StartWave();
	}

	return eMessageReturn::eContinue;
}

eMessageReturn CWaveManager::DoEvent(const CResetToCheckPointMessage & aResetToCheckpointMessage)
{
	myWaveCount = myResetToWaveCount;
	myNumberOfPlayers = 0;
	myNumberOfWavesToSpawn = myResetToNumberOfWaves;
	SComponentMessageData data; data.myInt = 10000;
	for (unsigned int i = 0; i < myEnemiesInWave.Size(); i++)
	{
		myEnemiesInWave[i]->GetParent()->NotifyComponents(eComponentMessageType::eDied, data);
		CNetworkMessage_SetIsRepesentationActive* deactivate = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_SetIsRepesentationActive>(ID_ALL);
		deactivate->SetActive(false);
		deactivate->SetNetworkID(myEnemiesInWave[i]->GetNetworkID());
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(deactivate));
	}

	myEnemiesInWave.RemoveAll();

	return eMessageReturn::eContinue;
}

eMessageReturn CWaveManager::DoEvent(const CSetAsNewCheckPointMessage& aSetAsNewCheckPointMessage)
{
	myResetToWaveCount = myWaveCount;
	myResetToNumberOfWaves = myNumberOfWavesToSpawn;

	return eMessageReturn::eContinue;
}
