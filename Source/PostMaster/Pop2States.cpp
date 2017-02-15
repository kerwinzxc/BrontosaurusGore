#include "stdafx.h"
#include "Pop2States.h"
#include "StateStack\StateStack.h"


Pop2States::Pop2States()
{
}


Pop2States::~Pop2States()
{
}

eMessageReturn Pop2States::DoEvent(StateStack * aStateStack) const
{
	if (aStateStack != nullptr)
	{
		
		auto Pop = [aStateStack]
		{
			aStateStack->Pop();
			aStateStack->Pop();

		};

		aStateStack->AddSwapStateFunction(Pop);
	}

	return eMessageReturn::eStop;
}
