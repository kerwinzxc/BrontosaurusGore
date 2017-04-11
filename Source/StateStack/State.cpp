#include "stdafx.h"
#include "State.h"

#include "../BrontosaurusEngine/Renderer.h"

State::State(StateStack& aStateStack, const eInputMessengerType aMessengerType, const int aPriority)
	: CInputMessenger(aMessengerType, aPriority)
	, myStateStack(aStateStack)
	, myStatus(eStateStatus::eKeep)
{
}

State::~State()
{
}

bool State::GetLetThroughRender() const
{
	return false;
}

bool State::GetLetThroughUpdate() const
{
	return false;
}

void State::SetStateStatus(const eStateStatus aStatus)
{
	myStatus = aStatus;
}