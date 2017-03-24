#include "RevivePlayerMessage.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"

CRevivePlayerMessage::CRevivePlayerMessage()
	: IMessage(eMessageType::eResetToCheckPointMessage)
{
}


CRevivePlayerMessage::~CRevivePlayerMessage()
{
}

Postmaster::Message::IMessage* CRevivePlayerMessage::Copy()
{
	return new CRevivePlayerMessage();
}

eMessageReturn CRevivePlayerMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
