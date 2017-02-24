#pragma once
#include "..\StateStack\State.h"
#include "../ThreadedPostmaster/Subscriber.h"

namespace GUI
{
	class GUIManager;
}

class LevelSelectState : public State, public Postmaster::ISubscriber
{
public:
	LevelSelectState(StateStack& aStateStack);
	~LevelSelectState();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;
	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;

	bool GetLetThroughRender() const override;

private:
	GUI::GUIManager* myGUIManager;
};
