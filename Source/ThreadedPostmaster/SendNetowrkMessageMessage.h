#pragma once
#include "Message.h"
#include "../TShared/NetworkMessage.h"
#include "../TShared/NetworkMessageHolder.h"

class CSendNetowrkMessageMessage: public Postmaster::Message::IMessage
{
public:
	CSendNetowrkMessageMessage(CNetworkMessage* aNetworkMessage);
	~CSendNetowrkMessageMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	CNetworkMessage* UnpackHolder() const;
private:


	SNetworkMessageHolder myNetworkMessage;

};

