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
	CMessageManager* messageManagerInstance = CClientMessageManager::GetInstance();
	if (messageManagerInstance == nullptr)
	{
		messageManagerInstance = CServerMessageManager::GetInstance();
		if (messageManagerInstance == nullptr)
		{
			DL_ASSERT("No message manager available");
		}
	}

	return messageManagerInstance->CreateMessage(myNetworkMessage);
}
