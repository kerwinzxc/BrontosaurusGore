#include "stdafx.h"
#include "SplashScreen.h"

#include "PostMaster\PostMaster.h"
#include "BrontosaurusEngine\SpriteInstance.h"
#include "PostMaster\Message.h"
#include "PostMaster\Event.h"

#define MAX_ALPHA 1.0f
#define MIN_APLHA 0.0f

CSplashScreenState::CSplashScreenState(StateStack& aStateStack)
	: State(aStateStack, eInputMessengerType::eSplashScreen)
	, myStayTime(1.5f)
{
	mySprites.Init(4);
	myFadeState = eFadeState::eFadingIn;
	myIsDone = false;
	myStayTimer = 0.f;
}

CSplashScreenState::~CSplashScreenState()
{
	mySprites.DeleteAll();
	myCurrentSprite = nullptr; //deleted in mySprites
}

eStateStatus CSplashScreenState::Update(const CU::Time& aDeltaTime)
{
	if (mySprites.Size() == 0) myIsDone = true;

	if (myIsDone == true)
	{
		myIsDone = false;

		if (CheckIfMorePicsInArray() == true)
			SetNextPic();
		else
			return eStateStatus::ePop;
	}

	myFadeState == eFadeState::eFadingIn ? FadeIn(aDeltaTime) : FadeOut(aDeltaTime);

	return eStateStatus::eKeep;
}

void CSplashScreenState::Render()
{
	if(myCurrentSprite != nullptr)
		myCurrentSprite->Render();
}

void CSplashScreenState::AddPicture(const char* aPath)
{	// 								             size           pos			Pivot				rect					 colour
	mySprites.Add(new CSpriteInstance(aPath, { 1.f, 1.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 0.f }));
}

void CSplashScreenState::UserWantsToContinue()
{
	if (CheckIfMorePicsInArray() == true)
	{
		SetNextPic();
	}
	else
	{
		myIsDone = true;
	}
}

void CSplashScreenState::FadeIn(const CU::Time& aDeltaTime)
{
	CU::Vector4f color = myCurrentSprite->GetColor();

	if (color.a < MAX_ALPHA)
		color.a += 0.5f * aDeltaTime.GetSeconds();

	if (color.a >= MAX_ALPHA)
	{
		myStayTimer += aDeltaTime.GetSeconds();
		if (myStayTimer >= myStayTime)
		{
			myFadeState = eFadeState::eFadingOut;
			myStayTimer = 0.f;
		}
	}

	myCurrentSprite->SetColor(color);
}

void CSplashScreenState::FadeOut(const CU::Time& aDeltaTime)
{
	CU::Vector4f color = myCurrentSprite->GetColor();

	if (color.a > MIN_APLHA)
		color.a -= 1.f * aDeltaTime.GetSeconds();
	else
		myIsDone = true;


	myCurrentSprite->SetColor(color);
}

void CSplashScreenState::SetNextPic()
{
	unsigned int index = mySprites.Find(myCurrentSprite) + 1;

	myCurrentSprite = mySprites[index];
	myFadeState = eFadeState::eFadingIn;
}

bool CSplashScreenState::CheckIfMorePicsInArray()
{
	if (mySprites.Find(myCurrentSprite) == mySprites.Size() - 1)
		return false;

	return true;
}

void CSplashScreenState::OnEnter(const bool /*aLetThroughRender*/)
{
	if (mySprites.Size() == 0) return;
	assert(mySprites.Size() > 0 && "You need to add something to the splashScreen");

	if (mySprites[0] != nullptr)
	{
		myCurrentSprite = mySprites[0];
		myCurrentSprite->SetColor({ 1.f, 1.f, 1.f, 1.f }); 
	}
}

void CSplashScreenState::OnExit(const bool /*aLetThroughRender*/)
{
}