#pragma once
#include "../ThreadedPostmaster/Message.h"

class CPlayerNetworkComponent;

class CSetClientIDMessage : public Postmaster::Message::IMessage
{
public:
	CSetClientIDMessage(CPlayerNetworkComponent& aComponent);
	~CSetClientIDMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	CPlayerNetworkComponent* GetPlayerComponent() const;

private:
	CPlayerNetworkComponent* myPlayerComponet;

};

