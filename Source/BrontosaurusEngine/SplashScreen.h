#pragma once
#include "stdafx.h"
#include "StateStack\State.h"

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

	State::eStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;
	void AddPicture(const char * aPath);

	eMessageReturn Recieve(const Message& aMessage) override;

private:
	void FadeIn(const CU::Time& aDeltaTime);
	void FadeOut(const CU::Time& aDeltaTime);
	void Init()    override {};
	void OnEnter() override;
	void OnExit()  override;

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

