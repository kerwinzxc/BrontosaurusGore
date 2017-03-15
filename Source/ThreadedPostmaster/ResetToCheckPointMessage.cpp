#include "ResetToCheckPointMessage.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"

CResetToCheckPointMessage::CResetToCheckPointMessage()
	: IMessage(eMessageType::eResetToCheckPointMessage)
{
}


CResetToCheckPointMessage::~CResetToCheckPointMessage()
{
}

Postmaster::Message::IMessage* CResetToCheckPointMessage::Copy()
{
	return new CResetToCheckPointMessage();
}

eMessageReturn CResetToCheckPointMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
