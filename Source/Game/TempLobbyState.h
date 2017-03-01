#pragma once
#include "StateStack/State.h"
#include "TextInstance.h"

enum class eLobbyState
{
	eEnterIpAndName,
	eSelectLevel,
};

class CTempLobbyState : public State
{
public:
	CTempLobbyState(StateStack& aStateStack);
	~CTempLobbyState();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	virtual void Render();

	virtual void OnEnter(const bool aLetThroughRender);
	virtual void OnExit(const bool aLetThroughRender);
	virtual bool GetLetThroughRender() const;
	virtual bool GetLetThroughUpdate() const;

private:
	CTextInstance myTextINstance;
	
	eLobbyState myLobbyState;
	unsigned char myCurrentLine;
};

