#include "stdafx.h"
#include "PopCurrentState.h"
#include "../StateStack/StateStack.h"

PopCurrentState::PopCurrentState() : IMessage(eMessageType::eStateStackMessage)
{
}

PopCurrentState::~PopCurrentState()
{
}

eMessageReturn PopCurrentState::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

Postmaster::Message::IMessage* PopCurrentState::Copy()
{
	return new PopCurrentState(*this);
}
