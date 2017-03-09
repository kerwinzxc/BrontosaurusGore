#include "stdafx.h"
#include "KeyCharPressed.h"
#include "BrontosaurusEngine\Console.h"

KeyCharPressed::KeyCharPressed(const char aKey)
	: IMessage(eMessageType::eCharPressed), myKey(aKey)
{
}

KeyCharPressed::~KeyCharPressed()
{
}

eMessageReturn KeyCharPressed::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

char KeyCharPressed::GetKey() const
{
	return myKey;
}

Postmaster::Message::IMessage* KeyCharPressed::Copy()
{
	return new KeyCharPressed(*this);
}
