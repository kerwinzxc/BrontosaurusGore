#include "stdafx.h"
#include "SendNetworkMessage.h"
#include "TClient/Client.h"


SendNetworkMessage::SendNetworkMessage(CNetworkMessage* aNetworkMessage) : myNetworkMessage(aNetworkMessage)
{
	
}


SendNetworkMessage::~SendNetworkMessage()
{
}

void SendNetworkMessage::SetPackage(CNetworkMessage* aMessage)
{
	myNetworkMessage = aMessage;
}

eMessageReturn SendNetworkMessage::DoEvent(CClient* aClient) const
{
	aClient->Send(myNetworkMessage);
	delete myNetworkMessage;
	return eMessageReturn::eStop;
}


