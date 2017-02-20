#pragma once
#include "../ThreadedPostmaster/Subscriber.h"
#include "../StateStack/State.h"

class CSpriteInstance;


class CreditsState : public State, public Postmaster::ISubscriber
{
public:
	CreditsState(StateStack& aStateStack,const bool aInGame = false);
	~CreditsState();

	// Inherited via State
	void Init() override;

	eStateStatus Update(const CU::Time & aDeltaTime) override;
	void Render() override;
	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	void GoToMainMenu();

private:
	CSpriteInstance* mysprite;
	const bool myInGame;
};

