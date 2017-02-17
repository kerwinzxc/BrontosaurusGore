#pragma once
#include "..\CommonUtilities\InputMessenger.h"

class StateStack;

namespace CU
{
	class Time;
}

enum class eStateStatus : unsigned char
{
	eKeep,
	ePop
};

enum class eInputMessengerType;

class State : protected CU::CInputMessenger
{
public:
	State(StateStack& aStateStack, const eInputMessengerType aMessengerType, const int aPriority = 0);
	State(const State& aCopy) = delete;
	virtual ~State();

	virtual void Init() = 0;
	virtual eStateStatus Update(const CU::Time& aDeltaTime) = 0;
	virtual void Render() = 0;

	virtual void OnEnter(const bool aLetThroughRender) = 0;
	virtual void OnExit(const bool aLetThroughRender) = 0;
	virtual bool GetLetThroughRender() const;
	virtual bool GetLetThroughUpdate() const;

	void SetStateStatus(const eStateStatus aStatus);

protected:
	StateStack& myStateStack;
	eStateStatus myStatus;
};
