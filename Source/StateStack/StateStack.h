#pragma once
#include "../PostMaster/Subscriber.h"
#include <functional>
#include "../ThreadedPostmaster/Subscriber.h"

class State;
enum class eStateStatus : unsigned char;

namespace CU
{
	class Time;
}

const bool STATESTACK_CONTINUE = true;
const bool STATESTACK_QUIT = false;

class StateStack : public Postmaster::ISubscriber
{
public:
	StateStack();
	~StateStack();
	void PushState(State *aState);
	State* GetCurrentState();
	bool Update(const CU::Time& aDeltaTime);
	void Render();
	void Pop();
	void Clear();

	void AddSwapStateFunction(const std::function<void(void)>& aSwapStateCunction);
	void SwapState(State* aState);

	inline void SetShouldUpdate(bool aShouldUpdate);

	eMessageReturn DoEvent(const ConsoleCalledUpon& aConsoleCalledUpon) override;
	eMessageReturn DoEvent(const PopCurrentState& aPopCurrent) override;
	eMessageReturn DoEvent(const ::PushState& aPushState) override;

	eMessageReturn DoEvent(const CChangeLevel& aChangeLevelMessage) override;
	eMessageReturn DoEvent(const CQuitGame& aQuitGameMessage) override;

private:
	const eStateStatus UpdateState(const CU::Time& aDeltaTime);
	void UpdateStateAtIndex(const CU::Time& aDeltaTime, const short aIndex);
	void RenderStateAtIndex(const short aIndex);



	CU::GrowingArray<State*, short> myStates;
	std::function<void(void)> mySwapStateFunction;

	State* myStateToSwapTo;
	bool myShouldUpdate;
};

inline void StateStack::SetShouldUpdate(bool aShouldUpdate)
{
	myShouldUpdate = aShouldUpdate;
}