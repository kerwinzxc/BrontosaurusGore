#pragma once
#include "Message.h"

class CConectedMessage: public Postmaster::Message::IMessage
{
public:
	CConectedMessage(const unsigned short anId);
	~CConectedMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const unsigned short myID;
};

