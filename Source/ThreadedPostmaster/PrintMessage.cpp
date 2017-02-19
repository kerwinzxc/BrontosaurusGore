#include "PrintMessage.h"
#include "Subscriber.h"
#include "Postmaster.h"

Postmaster::Message::CPrintMessage::CPrintMessage(const std::string& aText) : IMessage(eMessageType::Print), myText(aText)
{

}

Postmaster::Message::CPrintMessage::~CPrintMessage()
{
}

Postmaster::Message::IMessage* Postmaster::Message::CPrintMessage::Copy()
{
	return new CPrintMessage(*this);
}

const std::string& Postmaster::Message::CPrintMessage::GetText() const
{
	return myText;
}

void Postmaster::Message::CPrintMessage::DoMessage(ISubscriber& aSubscriber)
{
	aSubscriber.DoMessage(*this);
}

