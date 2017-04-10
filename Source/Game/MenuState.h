#pragma once
#include "StateStack/State.h"
#include "GUI/MenuManager.h"

class CMenuState :State
{
public:
	CMenuState(StateStack& aStateStack);
	~CMenuState();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;

	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	bool GetLetThroughRender() const override;
	bool GetLetThroughUpdate() const override;

	CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;
private:
	
	bool myShowStateBelow;

	CMenuManager myManager;
};

inline bool CMenuState::GetLetThroughRender() const
{
	return myShowStateBelow;
}

inline bool CMenuState::GetLetThroughUpdate() const
{
	return false;
}

