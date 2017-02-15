#include "stdafx.h"
#include "ChangeLevel.h"
#include "Game\LoadState.h"
#include "..\StateStack\StateStack.h"
#include "..\Game\PlayState.h"

ChangeLevel::ChangeLevel(const int aNewLevelIndex)
	: myNewLevelIndex(aNewLevelIndex)
{

}

ChangeLevel::~ChangeLevel()
{
}

eMessageReturn ChangeLevel::DoEvent(StateStack* aStateStack) const
{
	if (aStateStack != nullptr)
	{
		aStateStack->SwapState(new CLoadState(*aStateStack, myNewLevelIndex));
	}

	return eMessageReturn::eStop;
}
