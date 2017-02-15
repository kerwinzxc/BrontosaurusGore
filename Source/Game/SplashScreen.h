#pragma once
#include "StateStack\State.h"
#include "PostMaster\Subscriber.h"

enum class eFadeState
{
	eFadingIn,
	eFadingOut,
};

class CSpriteInstance;

class CSplashScreen : public State, public Subscriber
{
public:
	CSplashScreen(StateStack& aStateStack);
	~CSplashScreen();

	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;
	void AddPicture(const char * aPath);
	void UserWantsToContinue();

	eMessageReturn Recieve(const Message& aMessage) override;

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

