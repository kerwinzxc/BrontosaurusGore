#pragma once
#include "Message.h"
#include "../TShared/NetworkMessage.h"
#include "../TShared/NetworkMessageHolder.h"

class CSendNetworkMessageMessage: public Postmaster::Message::IMessage
{
public:
	CSendNetworkMessageMessage(CNetworkMessage* aNetworkMessage);
	~CSendNetworkMessageMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	CNetworkMessage* UnpackHolder() const;
private:
	CSendNetworkMessageMessage(SNetworkMessageHolder aNetworkMessage);

	SNetworkMessageHolder myNetworkMessage;

};

