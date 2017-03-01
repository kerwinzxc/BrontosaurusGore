#pragma once
#include "../ThreadedPostmaster/Message.h"
#include "../PostMaster/MessageType.h"

class CServerReadyMessage: public Postmaster::Message::IMessage
{
public:
	CServerReadyMessage(eMessageType aMessageType = eMessageType::eNetworkMessage);
	~CServerReadyMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
};
