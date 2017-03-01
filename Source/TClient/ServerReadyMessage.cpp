#include "stdafx.h"
#include "ServerReadyMessage.h"
#include "../ThreadedPostmaster/Subscriber.h"


CServerReadyMessage::CServerReadyMessage(eMessageType aMessageType): IMessage(aMessageType)
{
}


CServerReadyMessage::~CServerReadyMessage()
{
}

Postmaster::Message::IMessage* CServerReadyMessage::Copy()
{
	return new CServerReadyMessage();
}

eMessageReturn CServerReadyMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
