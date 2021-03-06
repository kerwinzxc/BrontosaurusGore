#include "stdafx.h"
#include "MessageManager.h"
#include "NetworkMessageHolder.h"
#include "NetworkMesssage_Connect.h"
#include "NetworkMessage_Ping.h"
#include "NetworkMessage_PingResponse.h"
#include "NetworkMessage_ConectResponse.h"
#include "NetworkMessage_ChatMessage.h"
#include "NetworkMessage_Position.h"
#include "NetworkMessage_LoadLevel.h"
#include "NetworkMessage_ClientReady.h"
#include "NetworkMessage_ServerReady.h"
#include "NetworkMessage_PlayerPositionMessage.h"
#include "NetworkMessage_WeaponShoot.h"
#include "NetworkMessage_PickupHealth.h"
#include "NetWorkMessage_PickupAmmo.h"
#include "Networkmessage_PickupArmor.h"
#include "NetworkMessage_PickupKey.h"
#include "NetworkMessage_PickupWeapon.h"
#include "NetworkMessage_EnemyPosition.h"
#include "NetworkMessage_EnemyTransformation.h"
#include "NetworkMessage_TakeDamage.h"
#include "NetworkMessage_OKShoot.h"
#include "NetworkMessage_TryToShoot.h"
#include "NetworkMessage_DoorMessage.h"
#include "NetworkMessage_SetCheckpointMessage.h"
#include "NetworkMessage_PlayerDied.h"
#include "NetworkMessage_PlayerRespawned.h"
#include "NetworkMessage_ResetToCheckpoint.h"
#include "NetworkMessage_RevivePlayer.h"
#include "NetworkMessage_SpawnEnemyRepesention.h"
#include "NetworkMessage_SetIsRepesentationActive.h"
#include "NetworkMessage_WeaponChange.h"
#include "NetworkMessage_AnimationStart.h"

CMessageManager::CMessageManager()
{
	CMessageStorage::CreateInstance();
}


CMessageManager::~CMessageManager()
{
	myMutex.lock();
	CMessageStorage::DestroyInstance();
	myMutex.unlock();
}

CNetworkMessage* CMessageManager::CreateMessage(const SNetworkMessageHolder& aMessageHolder)
{
	CNetworkMessage* message = CreateMessage(aMessageHolder.myHeader);

	message->SetData(aMessageHolder.Stream);
	return message;
}

CNetworkMessage* CMessageManager::CreateMessage(const SNetworkPackageHeader& aHeader)
{
	switch (static_cast<ePackageType>(aHeader.myPackageType))
	{
	case ePackageType::eConnect:
		return CreateMessage<CNetworkMessage_Connect>(aHeader);
	case ePackageType::ePing:
		return CreateMessage<CNetworkMessage_Ping>(aHeader);
	case ePackageType::ePingResponse: 
		return CreateMessage<CNetworkMessage_PingResponse>(aHeader);
	case ePackageType::eConnectResponse:
		return CreateMessage<CNetworkMessage_ConectResponse>(aHeader);
	case ePackageType::eChat: 
		return CreateMessage<CNetworkMessage_ChatMessage>(aHeader);
	case ePackageType::ePosition:
		return  CreateMessage<CNetworkMessage_Position>(aHeader);
	case ePackageType::eImportantResponse: 
		return  CreateMessage<CImportantNetworkMessage>(aHeader);
	case ePackageType::eLoadLevel: 
		return CreateMessage<CNetworkMessage_LoadLevel>(aHeader);
	case ePackageType::eClientReady: 
		return CreateMessage<CNetworkMessage_ClientReady>(aHeader);
	case ePackageType::eServerReady: 
		return CreateMessage<CNetworkMessage_ServerReady>(aHeader);
	case ePackageType::ePlayerPosition:
		return CreateMessage<CNetworkMessage_PlayerPositionMessage>(aHeader);
	case ePackageType::eWeaponShoot:
		return CreateMessage<CNetworkMessage_WeaponShoot>(aHeader);
	case ePackageType::ePickupHealth:
		return CreateMessage<CNetworkMessage_PickupHealth>(aHeader);
	case ePackageType::ePickupAmmo:
		return CreateMessage<CNetWorkMessage_PickupAmmo>(aHeader);
	case ePackageType::ePickupArmor:
		return CreateMessage<CNetworkmessage_PickupArmor>(aHeader);
	case ePackageType::ePickupKey:
		return CreateMessage<CNetworkMessage_PickupKey>(aHeader);
	case ePackageType::ePickupWeapon:
		return CreateMessage<CNetworkMessage_PickupWeapon>(aHeader);
	case ePackageType::eEnemyPosition:
		return CreateMessage<CNetworkMessage_EnemyPosition>(aHeader);
	case ePackageType::eEnemyTransformaion:
		return CreateMessage<CNetworkMessage_EnemyTransformation>(aHeader);
	case ePackageType::eTakeDamage:
		return CreateMessage<CNetworkMessage_TakeDamage>(aHeader);
	case ePackageType::eTryToShoot:
		return CreateMessage<CNetworkMessage_TryToShoot>(aHeader);
	case ePackageType::eOKShoot:
		return CreateMessage<CNetworkMessage_OKShoot>(aHeader);
	case ePackageType::eDoorMessage:
		return CreateMessage<CNetworkMessage_DoorMessage>(aHeader);
	case ePackageType::eSetCheckpointMessage:
		return CreateMessage<CNetworkMessage_SetCheckpointMessage>(aHeader);
	case ePackageType::ePlayerRespawned:
		return CreateMessage<CNetworkMessage_PlayerRespawned>(aHeader);
	case ePackageType::ePlayerDied:
		return CreateMessage<CNetworkMessage_PlayerDied>(aHeader);
	case ePackageType::eResetToCheckpoint:
		return CreateMessage<CNetworkMessage_ResetToCheckpoint>(aHeader);
	case ePackageType::eRevivePlayer:
		return CreateMessage<CNetworkMessage_RevivePlayer>(aHeader);
	case ePackageType::eSpawnEnemyRepresentation:
		return CreateMessage<CNetworkMessage_SpawnEnemyRepesention>(aHeader);
	case ePackageType::eSetRepesentationActive:
		return CreateMessage<CNetworkMessage_SetIsRepesentationActive>(aHeader);
	case ePackageType::eWeaponChange:
		return CreateMessage<CNetworkMessage_WeaponChange>(aHeader);
	case ePackageType::eAnimationStart:
		return CreateMessage<CNetworkMessage_AnimationStart>(aHeader);
	case ePackageType::eSize:
	case ePackageType::eZero:
	default: 
		DL_ASSERT("Your type seems to be missing int this switch please add it here when this crash happens");
		return nullptr;
	}
}
