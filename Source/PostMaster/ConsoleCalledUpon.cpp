#include "stdafx.h"
#include "ConsoleCalledUpon.h"
#include "StateStack\StateStack.h"

ConsoleCalledUpon::ConsoleCalledUpon(bool aIsConsoleActive)
{
	myIsConsoleActive = aIsConsoleActive;
}


ConsoleCalledUpon::~ConsoleCalledUpon()
{
}

eMessageReturn ConsoleCalledUpon::DoEvent(StateStack* aStateStack) const
{
	if (aStateStack != nullptr)
	{
		aStateStack->SetShouldUpdate(!myIsConsoleActive);
	}

	return eMessageReturn::eContinue;
}
