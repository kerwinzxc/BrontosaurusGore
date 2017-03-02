#include "SendNetowrkMessageMessage.h"
#include "../PostMaster/MessageType.h"
#include "../TClient/Client.h"
#include "../TClient/ClientMessageManager.h"
#include "../TShared/NetworkMessage_ClientReady.h"
#include "../TShared/NetworkMessage_LoadLevel.h"


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
	if (messageManagerInstance == nullptr)
	{
		return nullptr;
	}

	switch (static_cast<ePackageType>(myNetworkMessage.myHeader.myPackageType))
	{
	case ePackageType::eClientReady:
		{
			CNetworkMessage_ClientReady* ClientReadyMessage = messageManagerInstance->CreateMessage<CNetworkMessage_ClientReady>(myNetworkMessage);
			return ClientReadyMessage;
		}
	case ePackageType::eLoadLevel:
		{
			CNetworkMessage_LoadLevel* message = messageManagerInstance->CreateMessage<CNetworkMessage_LoadLevel>(myNetworkMessage);
			return message;
		}
	default: 
		return nullptr;
	}
}
