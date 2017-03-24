#pragma once
#include "../ThreadedPostmaster/Message.h"
class CRevivePlayerMessage : public Postmaster::Message::IMessage
{
public:
	CRevivePlayerMessage();
	~CRevivePlayerMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
};

