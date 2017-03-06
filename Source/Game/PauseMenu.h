#pragma once
#include "../StateStack/State.h"

class CSpriteInstance;

class CPauseMenuState : public State
{
public:
	CPauseMenuState(StateStack& aStateStack);
	~CPauseMenuState();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;
	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	bool GetLetThroughRender() const override;

	void Resume();
	void LevelSelect();
	void MainMenu();
	void Quit();

	CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;

private:
	CSpriteInstance* myPauseScreenSprite;
};
