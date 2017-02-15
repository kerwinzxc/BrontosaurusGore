#include "stdafx.h"
#include "StateStack.h"

StateStack::StateStack()
	: myStates(8)
	, mySwapStateFunction(nullptr)
	, myStateToSwapTo(nullptr)
	, myShouldUpdate(true)
{
	PostMaster::GetInstance().Subscribe(this, eMessageType::eStateStackMessage);
	PostMaster::GetInstance().Subscribe(this, eMessageType::eConsoleCalledUpon);
}

StateStack::~StateStack()
{
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eStateStackMessage);
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eConsoleCalledUpon);
	Clear();
}

void StateStack::PushState(State *aState)
{
	if(myShouldUpdate == true)
	{
		if (myStates.Size() > 0)
		{
			myStates.GetLast()->OnExit(aState->GetLetThroughRender());
		}

		myStates.Add(aState);
		aState->Init();
		aState->OnEnter(false);
	}
}

State* StateStack::GetCurrentState()
{
	return myStates.GetLast();
}

const eStateStatus StateStack::UpdateState(const CU::Time& aDeltaTime)
{
	if (myStates.GetLast()->GetLetThroughUpdate() == true)
	{
		UpdateStateAtIndex(aDeltaTime, myStates.Size() - 2);
	}

	return myStates.GetLast()->Update(aDeltaTime);
}

void StateStack::UpdateStateAtIndex(const CU::Time& aDeltaTime, const short aIndex)
{
	if (aIndex <= 0) return;

	if (myStates[aIndex]->GetLetThroughUpdate() == true && aIndex > 0)
	{
		UpdateStateAtIndex(aDeltaTime, aIndex - 1);
	}

	myStates[aIndex]->Update(aDeltaTime);
}

void StateStack::RenderStateAtIndex(const short aIndex)
{
	if (aIndex <= 0) return;

	if (myStates[aIndex]->GetLetThroughRender() == true && aIndex > 0)
	{
		RenderStateAtIndex(aIndex - 1);
	}

	myStates[aIndex]->Render();
}

bool StateStack::Update(const CU::Time& aDeltaTime)
{
	if (myStates.Size() > 0)
	{
		if (myShouldUpdate == true)
		{
			if (UpdateState(aDeltaTime) == eStateStatus::ePop)
			{
				Pop();
			}

			if (mySwapStateFunction != nullptr)
			{
				mySwapStateFunction();
				mySwapStateFunction = nullptr;
			}

			if (myStateToSwapTo != nullptr)
			{
				Pop();
				PushState(myStateToSwapTo);
				myStateToSwapTo = nullptr;
			}
		
		}

		return STATESTACK_CONTINUE;
	}

	return STATESTACK_QUIT;
}

void StateStack::Render()
{
	if (myStates.Size() > 0)
	{
		if (myStates.GetLast()->GetLetThroughRender() == true)
		{
			RenderStateAtIndex(myStates.Size() - 2);
		}

		myStates.GetLast()->Render();
	}
}

void StateStack::Pop()
{
	myStates.GetLast()->OnExit(false);
	bool letThroughRender = myStates.GetLast()->GetLetThroughRender();
	delete myStates.Pop();

	if (myStates.Size() > 0)
	{
		myStates.GetLast()->OnEnter(letThroughRender);
	}
}

void StateStack::Clear()
{
	while (myStates.Size() > 0)
	{
		myStates.GetLast()->OnExit(false);
		delete myStates.Pop();
	}
}

void StateStack::AddSwapStateFunction(const std::function<void(void)>& aSwapStateCunction)
{
	mySwapStateFunction = aSwapStateCunction;
}

void StateStack::SwapState(State* aState)
{
	myStateToSwapTo = aState;
}

eMessageReturn StateStack::Recieve(const Message& aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

