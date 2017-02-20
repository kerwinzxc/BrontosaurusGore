#pragma once
#include "Event.h"
#include "ThreadedPostmaster/Message.h"

class PopCurrentState : public Postmaster::Message::IMessage
{
public:
	PopCurrentState();
	~PopCurrentState();

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	IMessage* Copy() override;
};

