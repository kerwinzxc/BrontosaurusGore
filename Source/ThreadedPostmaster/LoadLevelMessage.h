#pragma once
#include "../ThreadedPostmaster/Message.h"

class CLoadLevelMessage: public Postmaster::Message::IMessage
{
public:
	CLoadLevelMessage(const int aLevelIndex);
	~CLoadLevelMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const int myLevelIndex;
private:
};

