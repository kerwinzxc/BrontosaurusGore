#include "stdafx.h"
#include "PushState.h"

#include "StateStack/StateStack.h"
#include "Game/PlayState.h"
#include "Game/CreditsState.h"
#include "Game/PauseMenu.h"
#include "Game/LevelSelectState.h"
#include "Game/MainMenuState.h"
#include "Game/LoadState.h"

PushState::PushState(eState aState, const int aLevelIndex)
	: myState(aState)
	, myLevelIndex(aLevelIndex)
{
}

PushState::~PushState()
{
}

eMessageReturn PushState::DoEvent(StateStack* aStateStack) const
{
	if (aStateStack != nullptr)
	{
		switch (myState)
		{
		case eState::ePlayState:
			aStateStack->PushState(new CLoadState(*aStateStack, myLevelIndex));
			break;
		case eState::eCreditScreen:
			aStateStack->PushState(new CreditsState(*aStateStack, static_cast<bool>(myLevelIndex)));
			break;
		case eState::ePauseScreen:
			aStateStack->PushState(new PauseMenu(*aStateStack));
			break;
		case eState::eLevelSelect:
			static_cast<MainMenuState*>(aStateStack->GetCurrentState())->SetIsGoingToLevelSelect(true);
			aStateStack->PushState(new LevelSelectState(*aStateStack));
			break;
		default: 
			break;
		}
	}

	return eMessageReturn::eStop;
}
