#pragma once
#include "Event.h"
#include "../ThreadedPostmaster/Message.h"

class KeyCharPressed : public Postmaster::Message::IMessage
{
public:
	KeyCharPressed(const char aKey);
	~KeyCharPressed();

	//eMessageReturn DoEvent(CConsole* aConsole) const override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	char GetKey() const;
	IMessage* Copy() override;
private:
	const char myKey;
};

