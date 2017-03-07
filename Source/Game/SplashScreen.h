#pragma once
#include "StateStack\State.h"
#include "CommonUtilities/GrowingArray.h"
#include "ThreadedPostmaster/Subscriber.h"

enum class eFadeState
{
	eFadingIn,
	eFadingOut,
};

class CSpriteInstance;

class CSplashScreenState : public State, public Postmaster::ISubscriber
{
public:
	CSplashScreenState(StateStack& aStateStack);
	~CSplashScreenState();

	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;
	void AddPicture(const char * aPath);
	void UserWantsToContinue();


private:
	void FadeIn(const CU::Time& aDeltaTime);
	void FadeOut(const CU::Time& aDeltaTime);
	void Init()    override {};
	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender)  override;

	void SetNextPic();
	bool CheckIfMorePicsInArray();

private:
	CU::GrowingArray<CSpriteInstance*> mySprites;
	CSpriteInstance* myCurrentSprite;
	eFadeState myFadeState;
	bool myIsDone;
	float myStayTimer;
	const float myStayTime;
};

