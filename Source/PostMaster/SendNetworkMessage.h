#pragma once
#include "Event.h"
#include "../TShared/NetworkMessage.h"

class SendNetworkMessage: public Event
{
public:

	SendNetworkMessage();
	~SendNetworkMessage();

	void SetPackage(CNetworkMessage* aMessage);

	virtual eMessageReturn DoEvent(CClient* aClient) const override;

private:

	CNetworkMessage* myNetworkMessage;
};

