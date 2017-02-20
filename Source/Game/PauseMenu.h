#pragma once
#include "../StateStack\State.h"
#include "../PostMaster/Subscriber.h"

namespace GUI
{
	class GUIManager;
}

class PauseMenu : public State/*, public Subscriber*/
{
public:
	PauseMenu(StateStack& aStateStack);
	~PauseMenu();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;
	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	eMessageReturn Recieve(const Message & aMessage);
	bool GetLetThroughRender() const override;

	void Resume();
	void LevelSelect();
	void MainMenu();
	void Quit();

private:
	GUI::GUIManager* myGUIManager;
};

