#include "stdafx.h"
#include "ConsoleCalledUpon.h"
#include "StateStack\StateStack.h"

ConsoleCalledUpon::ConsoleCalledUpon(bool aIsConsoleActive) : IMessage(eMessageType::eConsoleCalledUpon)
{
	myIsConsoleActive = aIsConsoleActive;
}


ConsoleCalledUpon::~ConsoleCalledUpon()
{
}

eMessageReturn ConsoleCalledUpon::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

bool ConsoleCalledUpon::GetIsConsoleActive() const
{
	return myIsConsoleActive;
}

Postmaster::Message::IMessage* ConsoleCalledUpon::Copy()
{
	return new ConsoleCalledUpon(*this);
}

//eMessageReturn ConsoleCalledUpon::DoEvent(StateStack* aStateStack) const
//{
//	if (aStateStack != nullptr)
//	{
//		aStateStack->SetShouldUpdate(!myIsConsoleActive);
//	}
//
//	return eMessageReturn::eContinue;
//}
