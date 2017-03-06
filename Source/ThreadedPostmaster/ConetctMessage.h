#pragma once
#include "Message.h"
#include <string>

class CConectMessage : public Postmaster::Message::IMessage
{
public:
	CConectMessage(const std::string& aName, const std::string& aIp);
	~CConectMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	const std::string myName;
	const std::string myIP;
};

