#include "ConetctMessage.h"
#include "../PostMaster/MessageType.h"
#include "Subscriber.h"





CConectMessage::CConectMessage(const std::string& aName, const std::string& aIp) : IMessage(eMessageType::eNetworkMessage), myName(aName), myIP(aIp)
{
}

CConectMessage::~CConectMessage()
{
}

Postmaster::Message::IMessage* CConectMessage::Copy()
{
	return new CConectMessage(myName, myIP);
}


eMessageReturn CConectMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
