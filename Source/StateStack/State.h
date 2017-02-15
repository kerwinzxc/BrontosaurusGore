#pragma once

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

class State
{
public:
	State(StateStack& aStateStack);

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
