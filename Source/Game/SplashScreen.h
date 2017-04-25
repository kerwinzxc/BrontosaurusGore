#pragma once
#include "StateStack\State.h"
#include "CommonUtilities/GrowingArray.h"
//#include "ThreadedPostmaster/Subscriber.h"

enum class eFadeState
{
	eFadingIn,
	eFadingOut,
};

class CSpriteInstance;

class CSplashScreenState : public State//, public Postmaster::ISubscriber
{
public:
	CSplashScreenState(StateStack& aStateStack);
	~CSplashScreenState();

	virtual void Init() override {};
	virtual void OnEnter(const bool aLetThroughRender) override;
	virtual void OnExit(const bool aLetThroughRender)  override;

	virtual eStateStatus Update(const CU::Time& aDeltaTime) override;
	virtual void Render() override;

	void AddPicture(const char* aPath);
	void UserWantsToContinue();

	virtual CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;

private:
	void FadeIn(const CU::Time& aDeltaTime);
	void FadeOut(const CU::Time& aDeltaTime);

	void SetNextPic();
	bool CheckIfMorePicsInArray();

private:
	CU::GrowingArray<CSpriteInstance*> mySprites;
	CSpriteInstance* myCurrentSprite;
	eFadeState myFadeState;
	float myStayTimer;
	const float myStayTime;
	bool myIsDone;
};

