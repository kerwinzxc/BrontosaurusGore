#include "SendNetowrkMessageMessage.h"
#include "../PostMaster/MessageType.h"
#include "../TClient/Client.h"
#include "../TClient/ClientMessageManager.h"
#include "../TServer/ServerMessageManager.h"
#include "../TShared/NetworkMessage_ClientReady.h"
#include "../TShared/NetworkMessage_LoadLevel.h"
#include "../TShared/NetworkMessage_Position.h"


CSendNetowrkMessageMessage::CSendNetowrkMessageMessage(CNetworkMessage* aNetworkMessage) : IMessage(eMessageType::eNetworkMessage)
{
	aNetworkMessage->PackMessage();
	myNetworkMessage.myHeader = aNetworkMessage->GetHeader();
	myNetworkMessage.Stream = aNetworkMessage->GetSerializedData();
}


CSendNetowrkMessageMessage::~CSendNetowrkMessageMessage()
{
}

Postmaster::Message::IMessage* CSendNetowrkMessageMessage::Copy()
{
	return new CSendNetowrkMessageMessage(UnpackHolder());
}

eMessageReturn CSendNetowrkMessageMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

CNetworkMessage* CSendNetowrkMessageMessage::UnpackHolder() const
{
	CClientMessageManager* messageManagerInstance = CClientMessageManager::GetInstance();
	CServerMessageManager* messageManagerInstance2 = CServerMessageManager::GetInstance();
	if (messageManagerInstance == nullptr)
	{
		if (messageManagerInstance2 == nullptr)
		{
			return nullptr;
		}
	}

	switch (static_cast<ePackageType>(myNetworkMessage.myHeader.myPackageType))
	{
	case ePackageType::eClientReady:
		{
			CNetworkMessage_ClientReady* ClientReadyMessage;
			if (messageManagerInstance2 == nullptr)
			{
				ClientReadyMessage = messageManagerInstance->CreateMessage<CNetworkMessage_ClientReady>(myNetworkMessage);
				return ClientReadyMessage;
			}
			ClientReadyMessage = messageManagerInstance2->CreateMessage<CNetworkMessage_ClientReady>(myNetworkMessage);
			return ClientReadyMessage;
		}
	case ePackageType::eLoadLevel:
		{
			CNetworkMessage_LoadLevel* message;
			if (messageManagerInstance2 == nullptr)
			{
				message = messageManagerInstance->CreateMessage<CNetworkMessage_LoadLevel>(myNetworkMessage);
				return message;
			}
			message = messageManagerInstance2->CreateMessage<CNetworkMessage_LoadLevel>(myNetworkMessage);
			return message;
		}
	case ePackageType::ePosition:
	{
		CNetworkMessage_Position* message;
		if (messageManagerInstance2 == nullptr)
		{
			message = messageManagerInstance->CreateMessage<CNetworkMessage_Position>(myNetworkMessage);
			return message;
		}
		message = messageManagerInstance2->CreateMessage<CNetworkMessage_Position>(myNetworkMessage);
		return message;
	}
	default: 
		return nullptr;
	}
}
