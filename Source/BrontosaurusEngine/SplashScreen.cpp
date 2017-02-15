#include "stdafx.h"
#include "SplashScreen.h"

#include "PostMaster\PostMaster.h"
#include "BrontosaurusEngine\SpriteInstance.h"
#include "PostMaster\Message.h"
#include "PostMaster\EMessageReturn.h"

#define MAX_ALPHA 1.0f
#define MIN_APLHA 0.0f

CSplashScreen::CSplashScreen(StateStack& aStateStack) : State(aStateStack) , myStayTime(1.5f)
{
	mySprites.Init(4);
	myFadeState = eFadeState::eFadingIn;
	myIsDone = false;
	myStayTimer = 0.f;
}

CSplashScreen::~CSplashScreen()
{
	mySprites.DeleteAll();
	myCurrentSprite = nullptr; //deleted in mySprites
}

State::eStatus CSplashScreen::Update(const CU::Time& aDeltaTime)
{
	if (myIsDone == true)
	{
		myIsDone = false;

		if (CheckIfMorePicsInArray() == true)
			SetNextPic();
		else
			return eStatus::ePop;
	}

	myFadeState == eFadeState::eFadingIn ? FadeIn(aDeltaTime) : FadeOut(aDeltaTime);

	return State::eStatus::eKeep;
}

void CSplashScreen::Render()
{
	if(myCurrentSprite != nullptr)
		myCurrentSprite->Render();
}

void CSplashScreen::AddPicture(const char* aPath)
{	// 								             size           pos                rect					 colour
	mySprites.Add(new CSpriteInstance(aPath, { 1.f, 1.f }, { 0.f, 0.f }, { 0.f, 0.f ,1.f, 1.f }, {1.f, 1.f, 1.f, 0.f}));
}

void CSplashScreen::FadeIn(const CU::Time& aDeltaTime)
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

void CSplashScreen::FadeOut(const CU::Time& aDeltaTime)
{
	CU::Vector4f color = myCurrentSprite->GetColor();

	if (color.a > MIN_APLHA)
		color.a -= 1.f * aDeltaTime.GetSeconds();
	else
		myIsDone = true;


	myCurrentSprite->SetColor(color);
}

void CSplashScreen::SetNextPic()
{
	unsigned char index = mySprites.Find(myCurrentSprite) + 1;

	myCurrentSprite = mySprites[index];
	myFadeState = eFadeState::eFadingIn;
}

bool CSplashScreen::CheckIfMorePicsInArray()
{
	if (mySprites.Find(myCurrentSprite) == mySprites.Size() - 1)
		return false;

	return true;
}

void CSplashScreen::OnEnter()
{
	POSTMASTER.AppendSubscriber(this, eMessageType::eKeyPressed);
	POSTMASTER.AppendSubscriber(this, eMessageType::eMousePressed);


	assert(mySprites.Size() > 0 && "You need to add something to the splashScreen");

	if (mySprites[0] != nullptr)
	{
		myCurrentSprite = mySprites[0];
		myCurrentSprite->SetColor({ 1.f, 1.f, 1.f, 1.f }); 
	}
}

void CSplashScreen::OnExit()
{
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eKeyPressed);
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eMousePressed);

}

eMessageReturn CSplashScreen::Recieve(const Message& aMessage)
{
	if (aMessage.myMessageType == eMessageType::eKeyPressed || aMessage.myMessageType ==  eMessageType::eMousePressed)
	{
		if (CheckIfMorePicsInArray() == true)
			SetNextPic();
		else
			myIsDone = true;
	}

	return eMessageReturn::eContinue;
}
