#pragma once
#include "Event.h"
#include "../ThreadedPostmaster/Message.h"

class ConsoleCalledUpon : public Postmaster::Message::IMessage
{
public:
	ConsoleCalledUpon(bool aIsConsoleActive);
	~ConsoleCalledUpon();

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	bool GetIsConsoleActive() const;
	IMessage* Copy() override;
	//eMessageReturn DoEvent(StateStack* aStateStack) const override;
private:
	bool myIsConsoleActive;
};

