#pragma once
#include "..\ThreadedPostmaster\Message.h"

class CQuitGame : public Postmaster::Message::IMessage
{
public:
	CQuitGame();
	~CQuitGame();

	IMessage* Copy() override;
	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
};

