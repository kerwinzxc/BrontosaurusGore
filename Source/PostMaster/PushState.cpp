#include "stdafx.h"
#include "PushState.h"

#include "StateStack/StateStack.h"
#include "Game/PlayState.h"
#include "Game/CreditsState.h"
#include "Game/PauseMenu.h"
#include "Game/LevelSelectState.h"
#include "Game/MainMenuState.h"
#include "Game/LoadState.h"

PushState::PushState(eState aState, const int aLevelIndex) : IMessage(eMessageType::eStateStackMessage)
	,myState(aState)
	, myLevelIndex(aLevelIndex)
{
}

PushState::~PushState()
{
}

eMessageReturn PushState::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

PushState::eState PushState::GetStateType() const
{
	return myState;
}

int PushState::GetLevelIndex() const
{
	return myLevelIndex;
}

Postmaster::Message::IMessage* PushState::Copy()
{
	return new PushState(*this);
}
