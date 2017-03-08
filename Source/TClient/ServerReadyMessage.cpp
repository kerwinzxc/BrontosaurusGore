#include "stdafx.h"
#include "ServerReadyMessage.h"
#include "../ThreadedPostmaster/Subscriber.h"


CServerReadyMessage::CServerReadyMessage(unsigned aNumberOfPlayers): IMessage(eMessageType::eNetworkMessage), myNumberOfPlayers(aNumberOfPlayers)
{
}


CServerReadyMessage::~CServerReadyMessage()
{
}

Postmaster::Message::IMessage* CServerReadyMessage::Copy()
{
	return new CServerReadyMessage(myNumberOfPlayers);
}

eMessageReturn CServerReadyMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
