#pragma once
#include "../StateStack/State.h"
#include "../PostMaster/Subscriber.h"

namespace GUI
{
	class GUIManager;
}

class CSkybox;

class MainMenuState : public State
{
public:
	MainMenuState(StateStack& aStateStack);
	~MainMenuState();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;
	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	bool GetLetThroughRender() const override;

	inline void SetIsGoingToLevelSelect(const bool aIsGoingToLevelSelect);

private:
	GUI::GUIManager* myGUIManager;
	CSkybox* mySkyBox;

	bool myIsGoingToLevelSelect;
};

inline void MainMenuState::SetIsGoingToLevelSelect(const bool aIsGoingToLevelSelect)
{
	myIsGoingToLevelSelect = aIsGoingToLevelSelect;
}
