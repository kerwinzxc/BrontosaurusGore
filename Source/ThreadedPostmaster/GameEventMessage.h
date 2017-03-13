
#pragma once
#include "../ThreadedPostmaster/Message.h"
#include <string>

class CGameEventMessage: public Postmaster::Message::IMessage
{
public:
	CGameEventMessage(const std::wstring& aMessage);
	~CGameEventMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const std::wstring myMessage;	
};
