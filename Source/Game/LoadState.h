#pragma once
#include "StateStack/State.h"
#include "LoadingAnimation.h"

class CPlayState;

class CLoadState :public State
{
public:
	CLoadState(StateStack& aStateStack, const int aLevelIndex);
	~CLoadState();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;

	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
private:

	CPlayState* myPlayState;
	CLoadingAnimation myLoadingAnimation;

	const int myLevelIndex;
};

