#pragma once
#include "Event.h"
#include "../TShared/NetworkMessage.h"

class SendNetworkMessage: public Event
{
public:

	SendNetworkMessage(CNetworkMessage* aNetworkMessage);
	~SendNetworkMessage();

	void SetPackage(CNetworkMessage* aMessage);

	virtual eMessageReturn DoEvent(CClient* aClient) const override;
	//virtual eMessageReturn DoEvent(CServerMain* aServer) const override;

private:

	CNetworkMessage* myNetworkMessage;
};

