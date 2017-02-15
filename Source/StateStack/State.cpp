#include "stdafx.h"
#include "State.h"

#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/Renderer.h"


State::State(StateStack& aStateStack)
	: myStateStack(aStateStack)
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
