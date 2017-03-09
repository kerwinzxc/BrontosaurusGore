#pragma once
#include "LoadingAnimation.h"
#include "../StateStack/State.h"
#include "../ThreadedPostmaster/Subscriber.h"

class StateStack;
class CPlayState;

class CLoadState :public State, Postmaster::ISubscriber
{
public:
	CLoadState(StateStack& aStateStack, const int aLevelIndex);
	~CLoadState();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;

	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;

	eMessageReturn DoEvent(const CServerReadyMessage& aSendNetowrkMessageMessage) override;
	eMessageReturn DoEvent(const CSpawnOtherPlayerMessage& aSpawnOtherPlayerMessage) override;
private:

	CPlayState* myPlayState;
	CLoadingAnimation myLoadingAnimation;

	const int myLevelIndex;
	bool myGotOkFromServer;

	short myNumberOfPlayersToSpawnBeforeStarting;
};

