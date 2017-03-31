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

CWaveManager::CWaveManager()
{
	myEnemiesInWave.Init(30);

	 myKeyIDToUnlock = 0;

	myPlayersInsideArena = 0;
	myWaveCount = 0;
	myNumberOfPlayers = 0;
	myNumberOfWavesToSpawn = 0;

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::ePlayerEnterArena);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eAddEnemyToWave);

}


CWaveManager::~CWaveManager()
{
}

void CWaveManager::StartWave()
{
	DL_PRINT(std::to_string(myNumberOfWavesToSpawn).c_str());
	DL_PRINT(std::to_string(myWaveCount).c_str());
	if (myWaveCount < myNumberOfWavesToSpawn)
	{
		myWaveCount++;
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CStartWaveMessage(myWaveCount));
	}
	else
	{
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
	for (int i = 0; i < myEnemiesInWave.Size(); i++)
	{
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
	myEnemiesInWave.Add(aAddEnemyToWave.GetEnemy());

	return eMessageReturn::eContinue;
}

eMessageReturn CWaveManager::DoEvent(const CPlayerEnteredArena & aPlayerEnteredArena)
{
	myPlayersInsideArena += aPlayerEnteredArena.GetPlayerChange();
	myKeyIDToUnlock = aPlayerEnteredArena.GetKeyId();
	myNumberOfWavesToSpawn = aPlayerEnteredArena.GetWaveAmount();

	if (myPlayersInsideArena >= myNumberOfPlayers)
	{
		CNetworkMessage_DoorMessage* door = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_DoorMessage>(ID_ALL);
		door->SetDoorAction(eDoorAction::eClose);
		door->SetKeyID(myKeyIDToUnlock);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(door));
		StartWave();
	}

	return eMessageReturn::eContinue;
}
