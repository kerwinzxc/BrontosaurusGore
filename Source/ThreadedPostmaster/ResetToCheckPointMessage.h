#pragma once
#include "../ThreadedPostmaster/Message.h"
class CResetToCheckPointMessage : public Postmaster::Message::IMessage
{
public:
	CResetToCheckPointMessage();
	~CResetToCheckPointMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
};

