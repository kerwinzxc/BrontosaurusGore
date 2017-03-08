#pragma once
#include "../ThreadedPostmaster/Message.h"
#include "../PostMaster/MessageType.h"

class CServerReadyMessage: public Postmaster::Message::IMessage
{
public:
	CServerReadyMessage(unsigned aNumberOfPlayers);
	~CServerReadyMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const unsigned myNumberOfPlayers;
};
