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

CMessageManager::CMessageManager()
{
	CMessageStorage::CreateInstance();
}


CMessageManager::~CMessageManager()
{
	CMessageStorage::DestroyInstance();
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
	case ePackageType::eSize:
	case ePackageType::eZero:
	default: 
		DL_ASSERT("Your type seems to be missing int this switch please add it here when this crash happens");
		return nullptr;
	}
}
