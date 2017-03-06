#include "PrintMessage.h"
#include "../PostMaster/MessageType.h"
#include "Subscriber.h"

Postmaster::Message::CPrintMessage::CPrintMessage(const std::string& aText) : IMessage(eMessageType::ePrint), myText(aText)
{
}

Postmaster::Message::CPrintMessage::~CPrintMessage()
{
}

const std::string& Postmaster::Message::CPrintMessage::GetText() const
{
	return myText;
}

Postmaster::Message::IMessage* Postmaster::Message::CPrintMessage::Copy()
{
	return new CPrintMessage(*this);
}

eMessageReturn Postmaster::Message::CPrintMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
